/*
 * Copyright (C) 2007, 2008, 2009 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Written by:
 * Dmitry Eremin-Solenikov <dbaryshkov@gmail.com>
 */

#ifndef WPAN_PHY_H
#define WPAN_PHY_H

#include <linux/netdevice.h>
#include <linux/mutex.h>
#include <linux/bug.h>

/* According to the IEEE 802.15.4 stadard the upper most significant bits of
 * the 32-bit channel bitmaps shall be used as an integer value to specify 32
 * possible channel pages. The lower 27 bits of the channel bit map shall be
 * used as a bit mask to specify channel numbers within a channel page.
 */
#define WPAN_NUM_CHANNELS	27
#define WPAN_NUM_PAGES		32

struct wpan_phy {
	struct mutex pib_lock;

	/*
	 * This is a PIB according to 802.15.4-2011.
	 * We do not provide timing-related variables, as they
	 * aren't used outside of driver
	 */
	u8 current_channel;
	u8 current_page;
	u32 channels_supported[32];
	s8 transmit_power;
	u8 cca_mode;
	u8 min_be;
	u8 max_be;
	u8 csma_retries;
	s8 frame_retries;

	bool lbt;
	s32 cca_ed_level;

	struct device dev;
	int idx;

	struct net_device *(*add_iface)(struct wpan_phy *phy,
					const char *name, int type);
	void (*del_iface)(struct wpan_phy *phy, struct net_device *dev);

	int (*set_txpower)(struct wpan_phy *phy, int db);
	int (*set_lbt)(struct wpan_phy *phy, bool on);
	int (*set_cca_mode)(struct wpan_phy *phy, u8 cca_mode);
	int (*set_cca_ed_level)(struct wpan_phy *phy, int level);
	int (*set_csma_params)(struct wpan_phy *phy, u8 min_be, u8 max_be,
			       u8 retries);
	int (*set_frame_retries)(struct wpan_phy *phy, s8 retries);

	char priv[0] __attribute__((__aligned__(NETDEV_ALIGN)));
};

#define to_phy(_dev)	container_of(_dev, struct wpan_phy, dev)

struct wpan_phy *wpan_phy_alloc(size_t priv_size);
static inline void wpan_phy_set_dev(struct wpan_phy *phy, struct device *dev)
{
	phy->dev.parent = dev;
}
int wpan_phy_register(struct wpan_phy *phy);
void wpan_phy_unregister(struct wpan_phy *phy);
void wpan_phy_free(struct wpan_phy *phy);
/* Same semantics as for class_for_each_device */
int wpan_phy_for_each(int (*fn)(struct wpan_phy *phy, void *data), void *data);

static inline void *wpan_phy_priv(struct wpan_phy *phy)
{
	BUG_ON(!phy);
	return &phy->priv;
}

struct wpan_phy *wpan_phy_find(const char *str);

static inline void wpan_phy_put(struct wpan_phy *phy)
{
	put_device(&phy->dev);
}

static inline const char *wpan_phy_name(struct wpan_phy *phy)
{
	return dev_name(&phy->dev);
}
#endif

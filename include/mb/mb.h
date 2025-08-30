/*******************************************************
 Mailbox rajapinnan määritykset
********************************************************/

#include <common/types.h>

#define MB_REQUEST  0

#define MB_TAG_GETSERIAL      0x10004
#define MB_TAG_LAST           0

#define MB_REQ_GETSERIAL    { 8*4, MB_REQUEST }

/****************** Board *****************************/
uint32_t mb_get_mac(uint8_t *mac);
uint32_t mb_get_sn(uint32_t *sn);

/**************** Framebuffer ************************/
uint32_t mb_fb_init(vector2_t *phy_res, vector2_t *vir_res, uint32_t depth, uint32_t address_size[2]);
uint32_t mb_fb_set_offset(uint32_t x, uint32_t y);

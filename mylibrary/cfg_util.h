#ifndef __BIM_CFG_DEF_H__
#define __BIM_CFG_DEF_H__

#include "cfg_loader.h"

#define  BIM_SYSTEM_CONFIG_BACKUP            "/etc/system.cfg"
#define  BIM_SNMP_CONFIG_BACKUP              "/etc/snmp.cfg"
#define  BIM_SYSTEM_CONFIG_ACTIVE_PATH       "/flash/config"
#define  BIM_SYSTEM_CONFIG_ACTIVE            BIM_SYSTEM_CONFIG_ACTIVE_PATH"/system.cfg"
#define  BIM_SNMP_CONFIG_ACTIVE              BIM_SYSTEM_CONFIG_ACTIVE_PATH"/snmp.cfg"
#define  BIM_SYSTEM_CONFIG_WEB_PATH          "/tmp/web"BIM_SYSTEM_CONFIG_ACTIVE_PATH
#define  BIM_SYSTEM_CONFIG_WEB               "/tmp/web"BIM_SYSTEM_CONFIG_ACTIVE
#define  BIM_SNMP_CONFIG_WEB                 "/tmp/web"BIM_SNMP_CONFIG_ACTIVE

#define  BIM_USER_CONFIG                     "/flash/config/bim_user.cfg"
/**
 * opens and parses a BIM system.cfg
 */
extern int open_bim_system_cfg(ConfigCB* cfg, int lock);
extern int update_bim_cfg(int lock, char* config_path, ...);
extern int read_bim_cfg(int lock, char* config_path, ...);

#endif //!__BIM_CFG_DEF_H__

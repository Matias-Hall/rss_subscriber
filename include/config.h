#ifndef _CONFIG_INC_GUARD
#define _CONFIG_INC_GUARD
/**
 * @brief Gets the path to the config folder.
 * It first tries $XDG_CONFIG_HOME, then $HOME/.config.
 * @returns A pointer to the config folder path, or 0 if none are found.
 */
const char *get_config_path(void);
/**
 * @brief Gets the path to the user data folder.
 * It first tries $XDG_DATA_HOME, then $HOME/.local/share.
 * @returns A pointer to the config folder path, or 0 if none are found.
 */
const char *get_data_path(void);
#endif /*_CONFIG_INC_GUARD*/

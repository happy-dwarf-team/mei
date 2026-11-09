/*
 * mei.text v1: Mei's service that tells another plugin how the game keeps its text.
 * Look it up with get_service(self, MEI_TEXT_SERVICE, MEI_TEXT_SERVICE_VERSION)
 * (anvil::Host::service in C++). Without Mei, or with a Mei from before this
 * service, it is NULL: the game's text is then Windows-1252.
 *
 * Mei registers it in its own load (phase "early"), so a plugin of phase "main"
 * finds it in anvil_plugin_load. Declare Mei as an optional dependency to keep
 * that order.
 *
 * Within version 1 functions are only appended to the end of MeiTextService:
 * check `size` before calling one (MEI_TEXT_HAS). Every function may be called
 * from any thread and never fails.
 *
 * Plain C11. SPDX-License-Identifier: MIT
 */
#ifndef MEI_TEXT_H
#define MEI_TEXT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MEI_TEXT_SERVICE "mei.text"
#define MEI_TEXT_SERVICE_VERSION 1

typedef struct MeiTextService {
    uint32_t size; /* sizeof(MeiTextService) of the Mei that registered it */
    /* Nonzero when the game keeps and draws its localisation in UTF-8 (Mei's
     * utf8_localization): text a plugin hands the game, such as a tooltip,
     * goes as UTF-8 and Mei draws it. Zero: the game's text is Windows-1252,
     * and a character it lacks shows as '?'. The answer holds for the process
     * (the setting takes effect at a restart). */
    int (*game_text_utf8)(void);
} MeiTextService;

/* Whether `service` (not NULL) has the field `member`. */
#define MEI_TEXT_HAS(service, member) \
    ((service)->size >= offsetof(MeiTextService, member) + sizeof((service)->member))

#ifdef __cplusplus
}
#endif

#endif /* MEI_TEXT_H */

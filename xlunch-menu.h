/*
 *      openbox-menu.h -  this file is part of openbox-menu
 *      Copyright (C) 2011,12 mimas <mimasgpc@free.fr>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU Lesser General Public License as published
 *      by the Free Software Foundation; version 3.0 only.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#define VERSION "0.5.0"

#define APPMENU_SIZE 30

guint app_is_visible(MenuCacheApp *, guint32);
gchar * get_item_icon_path(MenuCacheItem *);
gchar * get_safe_name(const char * name);
gchar * clean_exec(MenuCacheApp * app);

typedef struct {
	/* Configuration */
	gchar     *output;
	guint32    show_flag;
	GString   *builder;      /* */
	gchar     *terminal_cmd; /* command to launch program in a terminal */
	gboolean   comment;      /* display description instead of name */
	gboolean   sn;           /* startup notification */
	gboolean   no_icons;     /* icons disabled */
    gchar     *run_on_update;
    gboolean   persistent;
    MenuCache *menu;
} OB_Menu;


typedef struct _adapter_t {
    void (*root_node_open)(OB_Menu * context);
    void (*root_node_close)(OB_Menu * context);

    void (*directory_node_open)(OB_Menu * context, MenuCacheApp * app);
    void (*directory_node_close)(OB_Menu * context, MenuCacheApp * app);

    void (*menuitem)(OB_Menu * context, MenuCacheApp *app);
    void (*separator)(OB_Menu * context);

    void (*error)(OB_Menu * context, const char * s);
} adapter_t;

#define DECLARE_ADAPTER(name, prefix) \
adapter_t name = {\
    root_node_open: prefix##root_node_open,\
    root_node_close: prefix##root_node_close,\
    directory_node_open: prefix##directory_node_open,\
    directory_node_close: prefix##directory_node_close,\
    menuitem: prefix##menuitem,\
    separator: prefix##separator,\
    error: prefix##error\
}


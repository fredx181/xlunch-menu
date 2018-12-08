//      openbox-menu - a dynamic menu for openbox
//      Copyright (C) 2010-12 mimas <mimasgpc@free.fr>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; version 3 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>
#ifdef WITH_ICONS
	#include <gtk/gtk.h>
#endif
#include <glib/gi18n.h>
#include <menu-cache.h>
#include <signal.h>
#include <locale.h>
#include <stdlib.h>

#include "xlunch-menu.h"

GMainLoop *loop = NULL;
#ifdef WITH_ICONS
	GtkIconTheme *icon_theme;
#endif

#include "adapter-ob.c"
#include "adapter-jwm.c"

static adapter_t * adapter = &adapter_ob;

static int icon_size = 16;

/* from lxsession */
void sig_term_handler (int sig)
{
	g_warning ("Aborting");
	g_main_loop_quit (loop);
}

/****f* openbox-menu/get_safe_name
 * FUNCTION
 *   Convert &, <, > and " signs to html entities
 *
 * OUTPUT
 *   A gchar that needs to be freed.
 ****/
gchar * get_safe_name (const char *name)
{
	g_return_val_if_fail (name != NULL, NULL);

	GString *cmd = g_string_sized_new (256);

	for (;*name; ++name)
	{
		switch(*name)
		{
			case '&':
				g_string_append (cmd, "&amp;");
				break;
			case '<':
				g_string_append (cmd, "&lt;");
				break;
			case '>':
				g_string_append (cmd, "&gt;");
				break;
			case '"':
				g_string_append (cmd, "&quote;");
				break;
			default:
				g_string_append_c (cmd, *name);
		}
	}
	return g_string_free (cmd, FALSE);
}


/****f* openbox-menu/clean_exec
 * FUNCTION
 *   Remove %f, %F, %u, %U, %i, %c, %k from exec field.
 *   None of theses codes are interesting to manage here.
 *   %i, %c and %k codes are implemented but don't ask why we need them. :)
 *
 * OUTPUT
 *   A gchar that needs to be freed.
 *
 * NOTES
 *   %d, %D, %n, %N, %v and %m are deprecated and should be removed.
 *
 * SEE ALSO
 *   http://standards.freedesktop.org/desktop-entry-spec/latest/ar01s06.html
 ****/
gchar * clean_exec (MenuCacheApp *app)
{
	gchar *filepath = NULL;
	const char *exec = menu_cache_app_get_exec (MENU_CACHE_APP(app));

	if (strchr(exec, '%') == NULL)
		return g_strchomp(g_strdup(exec));

	GString *cmd = g_string_sized_new (64);

	for (;*exec; ++exec)
	{
		if (*exec == '%')
		{
			++exec;
			switch (*exec)
			{
				/* useless and commonly used codes */
				case 'u':
				case 'U':
				case 'f':
				case 'F': break;
				/* deprecated codes */
				case 'd':
				case 'D':
				case 'm':
				case 'n':
				case 'N':
				case 'v': break;
				/* Other codes, more or less pointless to implement */
				case 'c':
					g_string_append (cmd, menu_cache_item_get_name (MENU_CACHE_ITEM(app)));
					break;
#if WITH_ICONS
				case 'i':
					if (get_item_icon_path (MENU_CACHE_ITEM(app)))
					{
						g_string_append_printf (cmd, "--icon %s",
						    get_item_icon_path (MENU_CACHE_ITEM(app)));
					}
					break;
#endif
				case 'k':
					filepath = menu_cache_item_get_file_path (MENU_CACHE_ITEM(app));
					if (filepath)
					{
						g_string_append (cmd, filepath);
						g_free (filepath);
					}
					break;
				/* It was not in the freedesktop specification. */
				default:
					g_string_append_c (cmd, '%');
					g_string_append_c (cmd, *exec);
					break;

			}
		}
		else
			g_string_append_c (cmd, *exec);
	}
	return g_strchomp (g_string_free (cmd, FALSE));
}

#if WITH_ICONS
/****f* openbox-menu/get_item_icon_path
 * OUTPUT
 *   return the path for the themed icon if item.
 *   If no icon found, it returns the "empty" icon path.
 *
 *   The returned string should be freed when no longer needed
 *
 * NOTES
 *   Imlib2, used by OpenBox to display icons, doesn't load SVG graphics.
 *   We have to use GTK_ICON_LOOKUP_NO_SVG flag to look up icons.
 *
 * TODO
 *   The "2nd fallback" is annoying, I have to think about this.
 ****/
gchar *
get_item_icon_path (MenuCacheItem *item)
{
	GtkIconInfo *icon_info = NULL;
	gchar *icon = NULL;
	gchar *tmp_name = NULL;

	const gchar *name = menu_cache_item_get_icon (MENU_CACHE_ITEM(item));

	if (name)
	{
		if (g_path_is_absolute (name))
			return g_strdup (name);

		/*  We remove the file extension as gtk_icon_theme_lookup_icon can't
		 *  lookup a theme icon for, ie, 'geany.png'. It has to be 'geany'.
		 */
		tmp_name = strndup (name, strrchr (name, '.') - name);
	#ifdef WITH_SVG
		icon_info = gtk_icon_theme_lookup_icon (icon_theme, tmp_name, icon_size, GTK_ICON_LOOKUP_GENERIC_FALLBACK);
	#else	
		icon_info = gtk_icon_theme_lookup_icon (icon_theme, tmp_name, icon_size, GTK_ICON_LOOKUP_NO_SVG | GTK_ICON_LOOKUP_GENERIC_FALLBACK);	
	#endif
		g_free (tmp_name);
	}

	if (!icon_info) /* 2nd fallback */
		icon_info = gtk_icon_theme_lookup_icon (icon_theme, "empty", icon_size, GTK_ICON_LOOKUP_NO_SVG);

    if (icon_info)
    {
        icon = g_strdup (gtk_icon_info_get_filename (icon_info));
        gtk_icon_info_free (icon_info);
    }

	return icon;
}
#endif /* WITH_ICONS */


guint
app_is_visible(MenuCacheApp *app, guint32 de_flag)
{
	gint32 flags = menu_cache_app_get_show_flags (app);

	if (flags < 0)
		return !(- flags & de_flag);
	else
		return menu_cache_app_get_is_visible(MENU_CACHE_APP(app), de_flag);
}

/****f* openbox-menu/generate_openbox_menu
 * FUNCTION
 *   main routine of menu creation.
 *
 * NOTES
 *   It calls itself when 'dir' type is MENU_CACHE_TYPE_DIR.
 ****/
void
generate_openbox_menu (MenuCacheDir *dir, OB_Menu *context)
{
	GSList *l = NULL;

	for (l = menu_cache_dir_get_children (dir); l; l = l->next)
		switch ((guint) menu_cache_item_get_type (MENU_CACHE_ITEM(l->data)))
		{
			case MENU_CACHE_TYPE_DIR:
                adapter->directory_node_open(context, l->data);
                generate_openbox_menu(MENU_CACHE_DIR(l->data), context);
                adapter->directory_node_close(context, l->data);
				break;

			case MENU_CACHE_TYPE_SEP:
                adapter->separator(context);
				break;

			case MENU_CACHE_TYPE_APP:
				if (app_is_visible (MENU_CACHE_APP(l->data), 0))
					adapter->menuitem(context, l->data);
		}
}


/****f* openbox-menu/display_menu
 * FUNCTION
 *   it begins and closes the menu content, write it into a file or
 *   display it.
 *
 * INPUTS
 *   * menu
 *   * file, the filename where the menu content should be written to.
 *     If file is 'NULL' then the menu content is displayed.
 *
 * NOTES
 *   A 16 KiB GString is allocated for the content of the pipemenu.
 *   This should be enough prevent too many allocations while building
 *   the XML.
 *
 *   The size of the XML file created is around 8 KB in my computer but
 *   I don't have a lot of applications installed.
 ****/
void
display_menu (MenuCache *menu, OB_Menu *context)
{
    context->builder = g_string_sized_new (16 * 1024);

    adapter->root_node_open(context);

	MenuCacheDir *dir = menu_cache_get_root_dir (menu);
	if (dir == NULL)
	{
        adapter->error(context, "Cannot get menu root dir");
		g_warning ("Cannot get menu root dir");
	}

    GSList *l = NULL;
    if (dir != NULL)
    	l = menu_cache_dir_get_children(dir);

	if (g_slist_length (l) != 0)
    {
		generate_openbox_menu(dir, context);
	}
	else
    {
        adapter->error(context, "Cannot create menu, check if the .menu file is correct");
		g_warning ("Cannot create menu, check if the .menu file is correct");
    }

    adapter->root_node_close(context);

	gchar *buff = g_string_free (context->builder, FALSE);

	/* Has menu content to be saved in a file ? */
	if (context->output)
	{
		if (!g_file_set_contents (context->output, buff, -1, NULL))
			g_warning ("Can't write to %s\n", context->output);
		else
{
			g_message ("wrote to %s", context->output);

char buf[4096];
sprintf(buf, "sort -fu %s > /tmp/_xl_entries_tmp_ && mv /tmp/_xl_entries_tmp_ %s", context->output, context->output);
system(buf);
}
	}
	else /* No, so it's displayed on screen */
		g_print ("%s", buff);

    if (context->persistent && context->run_on_update && strlen(context->run_on_update))
    {
        g_spawn_command_line_async(context->run_on_update, NULL);
    }

	g_free (buff);

}

static guint reload_timeout_id = 0;

static gboolean on_reload_timeout(OB_Menu *context)
{
    display_menu(context->menu, context);
    reload_timeout_id = 0;
    return FALSE;
}

void on_reload_notify(MenuCache *menu, OB_Menu *context)
{
    context->menu = menu;
    if (reload_timeout_id == 0)
        reload_timeout_id = g_timeout_add(1500, on_reload_timeout, context);
}

gchar *
get_application_menu (void)
{
	gchar menu[APPMENU_SIZE];

	gchar *xdg_prefix = getenv("XDG_MENU_PREFIX");
	if (xdg_prefix)
	{
		g_snprintf (menu, APPMENU_SIZE, "%sapplications.menu", xdg_prefix);
	}
	else
		g_strlcpy (menu, "applications.menu", APPMENU_SIZE);

	return strdup (menu);
}


gboolean
check_application_menu (gchar *menu, OB_Menu *context)
{
	gchar *menu_path = (*menu == '/') ? g_strdup(menu) : g_build_filename ("/etc","xdg", "menus", menu, NULL);
	if (!g_file_test (menu_path, G_FILE_TEST_EXISTS))
	{

        g_warning("File %s doesn't exists. Can't create menu\n", menu_path);

        context->builder = g_string_sized_new (16 * 1024);
        adapter->root_node_open(context);
        adapter->error(context, "Menu not found");
        //adapter->error(context, "File %s doesn't exists", menu_path); FIXME
        adapter->root_node_close(context);

        gchar * buff = g_string_free(context->builder, FALSE);

        if (context->output)
            g_file_set_contents(context->output, buff, -1, NULL);
        else
            g_print ("%s", buff);

        g_free (buff);

		g_free (menu_path);
		return FALSE;
	}
	else
	{
		g_free (menu_path);
		return TRUE;
	}
}


int
main (int argc, char **argv)
{

	gpointer reload_notify_id = NULL;
	GError *error = NULL;
	gchar  *menu = NULL;
	OB_Menu ob_context = { 0 };

	gboolean  show_gnome = FALSE;
	gboolean  show_kde = FALSE;
	gboolean  show_xfce = FALSE;
	gboolean  show_rox = FALSE;
	gboolean  persistent = FALSE;
	gchar   **app_menu = NULL;
	gchar    *output = NULL;
	gchar    *terminal = "xterm -e";
	gchar    *format = "openbox";
    gchar    *run_on_update = NULL;

	/*
	 * TODO: Registered OnlyShowIn Environments
	 *  Ref: http://standards.freedesktop.org/menu-spec/latest/apb.html
	 *
	 * GNOME GNOME Desktop
	 * KDE   KDE Desktop
	 * LXDE  LXDE Desktop
	 * MATE  MATÉ Desktop
	 * Razor Razor-qt Desktop
	 * ROX   ROX Desktop
	 * TDE   Trinity Desktop
	 * Unity Unity Shell
	 * XFCE  XFCE Desktop
	 * Old   Legacy menu systems
	 */
	GOptionEntry entries[] = {
		{ "comment",   'c', 0, G_OPTION_ARG_NONE, &ob_context.comment,
		  "Show generic name instead of application name", NULL },
		{ "terminal",  't', 0, G_OPTION_ARG_STRING, &terminal,
		  "Terminal command (default xterm -e)", "cmd" },
		{ "gnome",     'g', 0, G_OPTION_ARG_NONE, &show_gnome,
		  "Show GNOME entries", NULL },
		{ "kde",       'k', 0, G_OPTION_ARG_NONE, &show_kde,
		  "Show KDE entries", NULL },
		{ "xfce",      'x', 0, G_OPTION_ARG_NONE, &show_xfce,
		  "Show XFCE entries", NULL },
		{ "rox",       'r', 0, G_OPTION_ARG_NONE, &show_rox,
		  "Show ROX entries", NULL },
		{ "persistent",'p', 0, G_OPTION_ARG_NONE, &persistent,
		  "stay active",    NULL },
		{ "run-on-update",'R', 0, G_OPTION_ARG_STRING, &run_on_update,
		  "command to run when menu changed (in persistent mode)",    NULL },
		{ "output",    'o', 0, G_OPTION_ARG_STRING, &output,
		  "file to write data to (e.g. /path/to/entries)", NULL },
		{ "icon-size",    'I', 0, G_OPTION_ARG_INT, &icon_size,
		  "desired icon size (16 by default)", NULL },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &app_menu,
		  NULL, "[/path/to/file.menu]" },
		{NULL}
	};
	GOptionContext *help_context = NULL;

	setlocale (LC_ALL, "");

	help_context = g_option_context_new (" - Xlunch menu generator " VERSION);
	g_option_context_set_help_enabled (help_context, TRUE);
	g_option_context_add_main_entries (help_context, entries, NULL);
	g_option_context_parse (help_context, &argc, &argv, &error);

	if (error)
	{
		g_print ("%s\n", error->message);
		g_error_free (error);
		return 1;
	}

	g_option_context_free (help_context);


        adapter = &adapter_jwm;


#ifdef WITH_ICONS
	if (!ob_context.no_icons)
	{
		gtk_init (&argc, &argv);
		icon_theme = gtk_icon_theme_get_default ();
	}
#endif

	if (output)
		ob_context.output = g_build_filename (output, NULL);

	else
		ob_context.output =  NULL;

	if (terminal)
		ob_context.terminal_cmd = terminal;

	if (show_gnome) ob_context.show_flag |= SHOW_IN_GNOME;
	if (show_kde)   ob_context.show_flag |= SHOW_IN_KDE;
	if (show_xfce)  ob_context.show_flag |= SHOW_IN_XFCE;
	if (show_rox)   ob_context.show_flag |= SHOW_IN_ROX;

	if (!app_menu)
		menu = get_application_menu ();
	else
		menu = strdup (*app_menu);

	if (!check_application_menu (menu, &ob_context))
		return 1;

	// wait for the menu to get ready
	MenuCache *menu_cache = menu_cache_lookup_sync (menu);
	if (!menu_cache )
	{
		g_free (menu);
		g_warning ("Cannot connect to menu-cache :/");
		return 1;
	}

	// display the menu anyway
	display_menu(menu_cache, &ob_context);

    ob_context.persistent = persistent;
    if (persistent)
        ob_context.run_on_update = run_on_update;


	if (persistent)
	{
		// menucache used to reload the cache after a call to menu_cache_lookup* ()
		// It's not true anymore with version >= 0.4.0.
		reload_notify_id = menu_cache_add_reload_notify (menu_cache,
		                        (MenuCacheReloadNotify) on_reload_notify,
		                        &ob_context);

		// install signals handler
		signal (SIGTERM, sig_term_handler);
		signal (SIGINT, sig_term_handler);

		// run main loop
		loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (loop);
		g_main_loop_unref (loop);

		menu_cache_remove_reload_notify (menu_cache, reload_notify_id);
	}

	menu_cache_unref (menu_cache);
	g_free (menu);

	g_free (ob_context.output);

	return 0;
}

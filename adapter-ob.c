

#define OB_ROOT_NODE_OPEN \
	"<openbox_pipe_menu xmlns=\"http://openbox.org/\""\
	"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	"  xsi:schemaLocation=\"http://openbox.org/"\
	"  file:///usr/share/openbox/menu.xsd\">\n"
#define OB_ROOT_NODE_CLOSE \
	"</openbox_pipe_menu>\n"


static void ob_root_node_open(OB_Menu * context)
{
    g_string_append(context->builder, OB_ROOT_NODE_OPEN);
}

static void ob_root_node_close(OB_Menu * context)
{
    g_string_append(context->builder, OB_ROOT_NODE_CLOSE);
}

static void ob_directory_node_open(OB_Menu * context, MenuCacheApp * dir)
{
	gchar *dir_id = get_safe_name (menu_cache_item_get_id (MENU_CACHE_ITEM(dir)));
	gchar *dir_name = get_safe_name (menu_cache_item_get_name (MENU_CACHE_ITEM(dir)));

#ifdef WITH_ICONS
	if (!context->no_icons)
	{
		gchar *dir_icon = get_item_icon_path (MENU_CACHE_ITEM(dir));

		g_string_append_printf (context->builder,
		    "<menu id=\"openbox-%s\" label=\"%s\" icon=\"%s\">\n",
		    dir_id, dir_name, dir_icon);
		g_free (dir_icon);
	}
	else
#endif
	{
		g_string_append_printf (context->builder,
	      "<menu id=\"openbox-%s\" label=\"%s\">\n",
	      dir_id, dir_name);
	}

	g_free (dir_id);
	g_free (dir_name);
}

static void ob_directory_node_close(OB_Menu * context, MenuCacheApp * app)
{
    g_string_append (context->builder, "</menu>\n");
}

static void ob_menuitem(OB_Menu * context, MenuCacheApp *app)
{
	gchar *exec_name = NULL;
	gchar *exec_icon = NULL;
	gchar *exec_cmd = NULL;

	/* is comment (description) or name displayed ? */
	if (context->comment && menu_cache_item_get_comment (MENU_CACHE_ITEM(app)))
		exec_name = get_safe_name (menu_cache_item_get_comment (MENU_CACHE_ITEM(app)));
	else
		exec_name = get_safe_name (menu_cache_item_get_name (MENU_CACHE_ITEM(app)));

	exec_cmd = clean_exec (app);

#ifdef WITH_ICONS
	if (!context->no_icons)
	{
		exec_icon = get_item_icon_path (MENU_CACHE_ITEM(app));
		g_string_append_printf (context->builder,
	      "<item label=\"%s\" icon=\"%s\"><action name=\"Execute\">",
	      exec_name,
	      exec_icon);
	}
	else
#endif
	{
		g_string_append_printf (context->builder,
	      "<item label=\"%s\"><action name=\"Execute\">",
	      exec_name);
	}

	if (context->sn && menu_cache_app_get_use_sn (app))
		g_string_append (context->builder,
	        "<startupnotify><enabled>yes</enabled></startupnotify>");

	if (menu_cache_app_get_use_terminal (app))
		g_string_append_printf (context->builder,
	        "<command><![CDATA[%s %s]]></command>\n</action></item>\n",
	        context->terminal_cmd,
	        exec_cmd);
	else
		g_string_append_printf (context->builder,
	        "<command><![CDATA[%s]]></command>\n</action></item>\n",
	        exec_cmd);

	g_free (exec_name);
	g_free (exec_icon);
	g_free (exec_cmd);
}

static void ob_separator(OB_Menu * context)
{
    g_string_append(context->builder, "<separator />\n");
}

static void ob_error(OB_Menu * context, const char * s)
{
    gchar * escaped = get_safe_name(s);
    g_string_append_printf(context->builder, "<item label=\"%s\"></item>\n", escaped);
    g_free(escaped);
}


DECLARE_ADAPTER(adapter_ob, ob_);




static void jwm_root_node_open(OB_Menu * context)
{
    g_string_append(context->builder, "");
}

static void jwm_root_node_close(OB_Menu * context)
{
    g_string_append(context->builder, "");
}

static void jwm_directory_node_open(OB_Menu * context, MenuCacheApp * dir)
{
	gchar *dir_name = get_safe_name (menu_cache_item_get_name (MENU_CACHE_ITEM(dir)));

#ifdef WITH_ICONS
	if (!context->no_icons)
	{
		gchar *dir_icon = get_item_icon_path (MENU_CACHE_ITEM(dir));

		g_string_append_printf (context->builder,
		    "",
		    dir_name, dir_icon);
		g_free (dir_icon);
	}
	else
#endif
	{
		g_string_append_printf (context->builder,
	      "",
	      dir_name);
	}

	g_free (dir_name);
}

static void jwm_directory_node_close(OB_Menu * context, MenuCacheApp * app)
{
    g_string_append (context->builder, "");
}

static void jwm_menuitem(OB_Menu * context, MenuCacheApp *app)
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
	      "%s;%s;",
	      exec_name,
	      exec_icon);
	}
	else
#endif
	{
		g_string_append_printf (context->builder,
	      "%s;",
	      exec_name);
	}

	if (menu_cache_app_get_use_terminal (app))
		g_string_append_printf (context->builder,
	        "%s %s",
	        context->terminal_cmd,
	        exec_cmd);
	else
		g_string_append_printf (context->builder,
	        "%s",
	        exec_cmd);

    g_string_append(context->builder, "\n");


	g_free (exec_name);
	g_free (exec_icon);
	g_free (exec_cmd);
}

static void jwm_separator(OB_Menu * context)
{
    g_string_append(context->builder, "");
}

static void jwm_error(OB_Menu * context, const char * s)
{
    gchar * escaped = get_safe_name(s);
    g_string_append_printf(context->builder, "%s;\n", escaped);
    g_free(escaped);
}


DECLARE_ADAPTER(adapter_jwm, jwm_);


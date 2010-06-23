/*
 * devchat_window.c
 * Copyright (C) Samuel Vincent Creshal 2010 <creshal@arcor.de>
 *
 * devchat-gui is free software: you can redistribute it and/or modify
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * devchat-gui is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "devchat_window.h"
#include "devchat_cb_data.h"

void notify_cb ();
void urlopen ();
void user_list_get();
void message_list_get();
void login_cb ();
void remote_level ();
void hotkey_cb ();
void destroy (GtkWidget* widget, DevchatCBData* data);
void login (GtkWidget* widget, DevchatCBData* data);
void config_cb (GtkWidget* widget, DevchatCBData* data);
void go_forum (GtkWidget* widget, DevchatCBData* data);
void close_tab (GtkWidget* widget, DevchatCBData* data);
void reconnect (GtkWidget* widget, DevchatCBData* data);
void tab_changed (GtkWidget* widget, DevchatCBData* data);
void tab_changed_win (GtkWidget* widget, DevchatCBData* data);
void on_motion (GtkWidget* widget, DevchatCBData* data);
void on_mark_set (GtkWidget* widget, DevchatCBData* data);
void level_changed (GtkWidget* widget, DevchatCBData* data);
void btn_format (GtkWidget* widget, DevchatCBData* data);
void btn_send (GtkWidget* widget, DevchatCBData* data);
void next_tab (GtkWidget* widget, DevchatCBData* data);
void prev_tab (GtkWidget* widget, DevchatCBData* data);
void show_his (GtkWidget* widget, DevchatCBData* data);
void about_cb (GtkWidget* widget, DevchatCBData* data);

gchar* current_time ();


G_DEFINE_TYPE (DevchatWindow, devchat_window, G_TYPE_OBJECT);

DevchatWindow*
devchat_window_new (void)
{
  DevchatWindow* window = g_object_new (DEVCHAT_TYPE_WINDOW, NULL);

  return window;
}

static void
devchat_window_init (DevchatWindow* self)
{
  GtkVBox* vbox0 = GTK_VBOX(gtk_vbox_new(FALSE,0));
  GtkWidget* menu = gtk_menu_bar_new();
  GtkWidget* hpaned1 = gtk_hpaned_new();

  self->smilies = g_hash_table_new (g_str_hash, g_str_equal);
  self->avatars = g_hash_table_new (g_str_hash, g_str_equal);
  self->settings.browser = g_strdup("<native>");
  self->settings.color_font = g_strdup("#a1aab2");
  self->settings.color_l1 = g_strdup("#2e3436");
  self->settings.color_l3 = g_strdup("#543535");
  self->settings.color_l5 = g_strdup("#354254");
  self->settings.color_l6 = g_strdup("#45513a");
  self->settings.color_greens = g_strdup("#8ae234");
  self->settings.color_blues = g_strdup("#729fcf");
  self->settings.color_time = g_strdup("#eeeeec");
  self->settings.color_url = g_strdup("#fce94f");
  self->settings.color_url_visited = g_strdup("#fcaf3e");
  self->settings.color_url_hover = g_strdup("#e9b96e");
  self->settings.color_highlight = g_strdup("#ef2929");
  self->settings.user = g_strdup(g_get_user_name());
  self->settings.pass = g_strdup("hidden");
  self->settings.showid = FALSE;
  self->settings.stealthjoin = FALSE;
  self->settings.autojoin = FALSE;
  self->settings.showhidden = FALSE;
  self->settings.coloruser = FALSE;
  self->settings.notify = g_strdup("<native>");
  self->settings.vnotify = g_strdup("<native>");
  self->settings.width = 600;
  self->settings.height = 400;
  self->settings.x = 0;
  self->settings.y = 0;
  self->settings.keywords = NULL; /*GSList*/
  self->settings.presets[0] = "";
  self->settings.presets[1] = "";
  self->settings.presets[2] = "";
  self->settings.presets[3] = "";
  self->settings.presets[4] = "";
  self->settings.presets[5] = "";
  self->settings.presets[6] = "";
  self->settings.presets[7] = "";
  self->settings.presets[8] = "";
  self->settings.presets[9] = "";
  self->firstrun = TRUE;
  self->hovertag = NULL;

  self->settings_backup = self->settings;

  DevchatCBData* self_data = devchat_cb_data_new (self, NULL);

  self->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW(self->window), g_strdup_printf ("%s %s",APPNAME, VERSION));
  gtk_widget_set_size_request (self->window, 600,400);
  gtk_window_move(GTK_WINDOW(self->window), self->settings.x, self->settings.y);
  self->accelgroup = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(self->window), self->accelgroup);
  g_signal_connect(self->window, "destroy", G_CALLBACK(destroy),&self_data);
  g_signal_connect(self->window, "focus-in-event", G_CALLBACK(tab_changed_win),self_data);


  GtkMenuItem* menu_main = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Main"));
  GtkMenuItem* menu_edit = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Edit"));
  GtkMenuItem* menu_insert = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Insert"));
  GtkMenuItem* menu_view = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Go"));
  GtkMenuItem* menu_about = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Help"));

  self->item_connect = gtk_image_menu_item_new_from_stock(GTK_STOCK_CONNECT,self->accelgroup);
  g_signal_connect (self->item_connect, "activate", G_CALLBACK (login),self_data);

  GtkWidget* item_prefs = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES,self->accelgroup);
  g_signal_connect (item_prefs, "activate", G_CALLBACK (config_cb),self_data);
  gtk_widget_add_accelerator(item_prefs, "activate", self->accelgroup, GDK_P, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* edit_profile = devchat_cb_data_new (self, GINT_TO_POINTER (URL_PROFILE_EDIT));

  GtkWidget* item_profile = gtk_image_menu_item_new_with_label ("Edit profile...");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_profile), gtk_image_new_from_icon_name("system-users",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_profile, "activate", G_CALLBACK (go_forum), edit_profile);

  DevchatCBData* view_devnet = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L3));

  self->item_l3 = gtk_menu_item_new_with_label ("Open DevNet fora...");
  g_signal_connect (self->item_l3, "activate", G_CALLBACK (go_forum), view_devnet);
  gtk_widget_set_no_show_all(self->item_l3,TRUE);

  DevchatCBData* view_beta = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L5));

  self->item_l5 = gtk_menu_item_new_with_label ("Open betatest fora...");
  g_signal_connect (self->item_l5, "activate", G_CALLBACK (go_forum), view_beta);
  gtk_widget_set_no_show_all(self->item_l5,TRUE);

  DevchatCBData* view_forum = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L1));

  GtkWidget* item_forum = gtk_menu_item_new_with_label ("Open forum...");
  g_signal_connect (item_forum, "activate", G_CALLBACK (go_forum), view_forum);

  GtkWidget* item_tabclose = gtk_image_menu_item_new_with_label ("Close tab");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tabclose), gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tabclose, "activate", G_CALLBACK (close_tab), self_data);
  gtk_widget_add_accelerator(item_tabclose, "activate", self->accelgroup, GDK_W, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  self->item_reconnect = gtk_image_menu_item_new_with_label ("Reconnect");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (self->item_reconnect), gtk_image_new_from_stock(GTK_STOCK_REFRESH,GTK_ICON_SIZE_MENU));
  g_signal_connect (self->item_reconnect, "activate", G_CALLBACK (reconnect), self_data);
  gtk_widget_set_no_show_all(self->item_reconnect,TRUE);
  gtk_widget_add_accelerator(self->item_reconnect, "activate", self->accelgroup, GDK_R, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_disconnect = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT,self->accelgroup);
  g_signal_connect (item_disconnect, "activate", G_CALLBACK (destroy), self_data);
  gtk_widget_add_accelerator(item_disconnect, "activate", self->accelgroup, GDK_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_tab_next = gtk_image_menu_item_new_with_label ("Next Tab");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tab_next), gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tab_next, "activate", G_CALLBACK (next_tab), self_data);
  gtk_widget_add_accelerator(item_tab_next, "activate", self->accelgroup, GDK_Page_Down, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(item_tab_next, "activate", self->accelgroup, GDK_Tab, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_tab_prev = gtk_image_menu_item_new_with_label ("Previous Tab");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tab_prev), gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tab_prev, "activate", G_CALLBACK (prev_tab), self_data);
  gtk_widget_add_accelerator(item_tab_prev, "activate", self->accelgroup, GDK_Page_Up, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_his = gtk_image_menu_item_new_with_label ("Show History...");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_his), gtk_image_new_from_icon_name("appointment-new",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_his, "activate", G_CALLBACK (show_his), self_data);

  GtkWidget* item_about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT,self->accelgroup);
  g_signal_connect (item_about, "activate", G_CALLBACK (about_cb), self_data);

  DevchatCBData* format_b = devchat_cb_data_new (self, g_strdup("b"));

  GtkWidget* item_bold = gtk_image_menu_item_new_from_stock (GTK_STOCK_BOLD,self->accelgroup);
  g_signal_connect (item_bold, "activate", G_CALLBACK (btn_format), format_b);
  gtk_widget_add_accelerator(item_bold, "activate", self->accelgroup, GDK_B, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_i = devchat_cb_data_new (self, g_strdup("i"));

  GtkWidget* item_italic = gtk_image_menu_item_new_from_stock (GTK_STOCK_ITALIC,self->accelgroup);
  g_signal_connect (item_italic, "activate", G_CALLBACK (btn_format), format_i);
  gtk_widget_add_accelerator(item_italic, "activate", self->accelgroup, GDK_I, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_u = devchat_cb_data_new (self, g_strdup("u"));

  GtkWidget* item_line = gtk_image_menu_item_new_from_stock (GTK_STOCK_UNDERLINE,self->accelgroup);
  g_signal_connect (item_line, "activate", G_CALLBACK (btn_format), format_u);
  gtk_widget_add_accelerator(item_line, "activate", self->accelgroup, GDK_U, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_img = devchat_cb_data_new (self, g_strdup("img"));

  GtkWidget* item_pict = gtk_image_menu_item_new_with_mnemonic ("I_mage");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_pict), gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_pict, "activate", G_CALLBACK (btn_format), format_img);
  gtk_widget_add_accelerator(item_pict, "activate", self->accelgroup, GDK_M, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_url = devchat_cb_data_new (self, g_strdup("url"));

  GtkWidget* item_link = gtk_image_menu_item_new_with_mnemonic ("_Link");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_link), gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_link, "activate", G_CALLBACK (btn_format), format_url);
  gtk_widget_add_accelerator(item_link, "activate", self->accelgroup, GDK_L, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_smilies = gtk_menu_item_new_with_mnemonic ("_Smilies...");
  /*TODO: Smilie-Menü füllen.*/
  GtkWidget* item_presets = gtk_menu_item_new_with_mnemonic ("_Preset texts...");
  /*TODO: Preset-Menü füllen.*/

  GtkMenuShell* main_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(main_sub, self->item_connect);
  gtk_menu_shell_append(main_sub, item_tabclose);
  gtk_menu_shell_append(main_sub, gtk_menu_item_new());
  gtk_menu_shell_append(main_sub, self->item_reconnect);
  gtk_menu_shell_append(main_sub, item_disconnect);
  gtk_menu_item_set_submenu(menu_main,GTK_WIDGET(main_sub));

  GtkMenuShell* insert_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(insert_sub, item_bold);
  gtk_menu_shell_append(insert_sub, item_italic);
  gtk_menu_shell_append(insert_sub, item_line);
  gtk_menu_shell_append(insert_sub, item_pict);
  gtk_menu_shell_append(insert_sub, item_link);
  gtk_menu_shell_append(insert_sub, gtk_menu_item_new());
  gtk_menu_shell_append(insert_sub, item_smilies);
  gtk_menu_shell_append(insert_sub, item_presets);
  gtk_menu_item_set_submenu(menu_insert,GTK_WIDGET(insert_sub));

  GtkMenuShell* edit_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(edit_sub, item_profile);
  gtk_menu_shell_append(edit_sub, gtk_menu_item_new());
  gtk_menu_shell_append(edit_sub, item_prefs);
  gtk_menu_item_set_submenu(menu_edit,GTK_WIDGET(edit_sub));

  GtkMenuShell* view_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(view_sub, item_tab_next);
  gtk_menu_shell_append(view_sub, item_tab_prev);
  gtk_menu_shell_append(view_sub, gtk_menu_item_new());
  gtk_menu_shell_append(view_sub, self->item_l3);
  gtk_menu_shell_append(view_sub, self->item_l5);
  gtk_menu_shell_append(view_sub, item_forum);
  gtk_menu_shell_append(view_sub, gtk_menu_item_new());
  gtk_menu_shell_append(view_sub, item_his);
  gtk_menu_item_set_submenu(menu_view,GTK_WIDGET(view_sub));

  GtkMenuShell* about_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(about_sub, item_about);
  gtk_menu_item_set_submenu(menu_about,GTK_WIDGET(about_sub));

  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_main));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_edit));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_insert));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_view));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_about));

  gtk_box_pack_start (GTK_BOX(vbox0), GTK_WIDGET(menu), FALSE,FALSE,0);


  self->notebook = gtk_notebook_new ();
  g_signal_connect(self->notebook, "switch-page", G_CALLBACK (tab_changed), self_data);
  gtk_box_pack_start(GTK_BOX(vbox0), self->notebook, TRUE,TRUE,0);
  gtk_container_add(GTK_CONTAINER(self->window),GTK_WIDGET(vbox0));

  self->statusbar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(self->statusbar),TRUE);
  self->statuslabel = gtk_label_new ("Not connected.");
  self->userlabel = gtk_label_new ("");
  gtk_box_pack_end ( GTK_BOX(self->statusbar), self->userlabel,FALSE,FALSE,0);
  gtk_box_pack_end ( GTK_BOX(self->statusbar), gtk_vseparator_new(),FALSE,FALSE,1);
  gtk_box_pack_end ( GTK_BOX(self->statusbar), self->statuslabel,FALSE,FALSE,0);
  gtk_box_pack_end ( GTK_BOX(self->statusbar), gtk_vseparator_new(),FALSE,FALSE,1);
  gtk_box_pack_start(GTK_BOX(vbox0), self->statusbar, FALSE,FALSE,1);

  self->output = gtk_text_buffer_new (NULL);
  self->outputwidget = gtk_text_view_new_with_buffer (self->output);
  GdkColor l1;
  GdkColor font;
  gdk_color_parse (self->settings.color_l1, &l1);
  gdk_color_parse (self->settings.color_font, &font);
  gtk_widget_modify_base (self->outputwidget, GTK_STATE_NORMAL, &l1);
  gtk_widget_modify_text (self->outputwidget, GTK_STATE_NORMAL, &font);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(self->outputwidget), FALSE);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(self->outputwidget), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(self->outputwidget), GTK_WRAP_WORD_CHAR);
  gtk_widget_set_size_request(self->outputwidget, 300,100);
  gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(self->outputwidget), 2);
  g_signal_connect (self->output, "mark-set", G_CALLBACK(on_mark_set),self);
  g_signal_connect (self->outputwidget, "motion-notify-event", G_CALLBACK(on_motion),self_data);

  GtkWidget* scroller1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller1),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller1),GTK_SHADOW_ETCHED_IN);
  gtk_container_add(GTK_CONTAINER(scroller1),self->outputwidget);
  gtk_paned_pack1 (GTK_PANED(hpaned1), scroller1, TRUE,TRUE);

  GtkWidget* scroller2 = gtk_scrolled_window_new (NULL, NULL);
  self->userlist = gtk_vbox_new (TRUE,1);
  gtk_widget_set_size_request (self->userlist, 180, -1);
  if (self->settings.coloruser)
    gtk_widget_modify_bg (scroller2, GTK_STATE_NORMAL, &l1);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(scroller2), self->userlist);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller2),GTK_SHADOW_ETCHED_IN);
  gtk_paned_pack2 (GTK_PANED(hpaned1), scroller2, FALSE, FALSE);


  GtkWidget* vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_size_request (vbox2, -1, 74);

  self->inputbar = gtk_hbox_new (FALSE, 0);
  self->level_box = gtk_combo_box_new_text ();
  gtk_widget_set_no_show_all (self->level_box,TRUE);
  g_signal_connect(self->level_box, "changed", G_CALLBACK (level_changed), self_data);
  gtk_box_pack_start(GTK_BOX(self->inputbar),self->level_box,FALSE,FALSE,0);
  GtkWidget* btn_bold = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_bold), gtk_image_new_from_stock(GTK_STOCK_BOLD,GTK_ICON_SIZE_SMALL_TOOLBAR));
  GtkWidget* btn_ital = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_ital), gtk_image_new_from_stock(GTK_STOCK_ITALIC,GTK_ICON_SIZE_SMALL_TOOLBAR));
  GtkWidget* btn_line = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_line), gtk_image_new_from_stock(GTK_STOCK_UNDERLINE,GTK_ICON_SIZE_SMALL_TOOLBAR));
  GtkWidget* btn_url = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_url), gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_SMALL_TOOLBAR));
  gtk_widget_set_tooltip_text (btn_url, "Insert URL");
  GtkWidget* btn_img = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_img), gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_SMALL_TOOLBAR));
  gtk_widget_set_tooltip_text (btn_img, "Insert Image\nNote that you will be killed if you insert images larger than 32*32px. You have been warned.");

  g_signal_connect (btn_bold, "clicked", G_CALLBACK (btn_format), format_b);
  g_signal_connect (btn_ital, "clicked", G_CALLBACK (btn_format), format_i);
  g_signal_connect (btn_line, "clicked", G_CALLBACK (btn_format), format_u);
  g_signal_connect (btn_url, "clicked", G_CALLBACK (btn_format), format_url);
  g_signal_connect (btn_img, "clicked", G_CALLBACK (btn_format), format_img);

  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_bold,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_ital,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_line,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_url,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_img,FALSE,FALSE,0);

  GtkWidget* btn_ok = gtk_button_new_from_stock(GTK_STOCK_OK);
  g_signal_connect (btn_ok, "clicked", G_CALLBACK (btn_send),self_data);
  gtk_widget_add_accelerator(btn_ok, "clicked", self->accelgroup, GDK_Return, 0, 0);

  GtkWidget* btn_quit = gtk_button_new_from_stock(GTK_STOCK_QUIT);
  g_signal_connect (btn_quit, "clicked", G_CALLBACK (destroy),self_data);
  gtk_widget_add_accelerator(btn_quit, "clicked", self->accelgroup, GDK_Q, GDK_CONTROL_MASK, 0);

  gtk_box_pack_end (GTK_BOX(self->inputbar),btn_quit,FALSE,FALSE,0);
  gtk_box_pack_end (GTK_BOX(self->inputbar),gtk_vseparator_new(),FALSE,FALSE,0);
  gtk_box_pack_end (GTK_BOX(self->inputbar),btn_ok,FALSE,FALSE,0);

  gtk_box_pack_start (GTK_BOX(vbox2),self->inputbar,FALSE,FALSE,0);


  GtkWidget* scroller3 = gtk_scrolled_window_new (NULL,NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller3),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller3),GTK_SHADOW_ETCHED_IN);

  self->input = gtk_text_buffer_new (NULL);
  self->inputwidget = gtk_text_view_new_with_buffer (self->input);
  gtk_widget_modify_base (self->inputwidget, GTK_STATE_NORMAL, &l1);
  gtk_widget_modify_text (self->inputwidget, GTK_STATE_NORMAL, &font);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(self->inputwidget), GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(self->inputwidget), 2);
  gtk_container_add(GTK_CONTAINER(scroller3),self->inputwidget);

  gtk_box_pack_start (GTK_BOX(vbox2),scroller3,TRUE,TRUE,0);

  /*TODO: Texttags für output erzeugen.*/

  self->loginbar = gtk_vbox_new (FALSE,0);
  GtkWidget* hbox2 = gtk_hbox_new (FALSE,0);
  self->user_entry = gtk_entry_new ();
  self->pass_entry = gtk_entry_new ();
  gtk_entry_set_text( GTK_ENTRY (self->user_entry), self->settings.user);
  gtk_entry_set_text( GTK_ENTRY (self->pass_entry), self->settings.pass);
  gtk_entry_set_visibility (GTK_ENTRY (self->pass_entry), FALSE);

  self->btn_connect = gtk_button_new_from_stock (GTK_STOCK_CONNECT);
  g_signal_connect (self->btn_connect, "clicked", G_CALLBACK (login), self_data);
  gtk_widget_add_accelerator(self->btn_connect, "activate", self->accelgroup, GDK_Return, 0, 0);

  gtk_box_pack_start (GTK_BOX(hbox2),self->user_entry,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX(hbox2),self->pass_entry,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX(hbox2),self->btn_connect,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (self->loginbar), hbox2,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), self->loginbar,FALSE,FALSE,0);

  GtkWidget* vpaned = gtk_vpaned_new ();
  gtk_paned_pack1 (GTK_PANED(vpaned),hpaned1,TRUE,TRUE);
  gtk_paned_pack2 (GTK_PANED(vpaned),vbox2,FALSE,FALSE);

  gtk_notebook_append_page_menu(GTK_NOTEBOOK(self->notebook),vpaned,gtk_label_new("X-DEVCHAT"),gtk_label_new("X-DEVCHAT"));

  gtk_widget_show_all (self->window);
  gtk_widget_hide_all (self->inputbar);

  gtk_window_resize (GTK_WINDOW (self->window), self->settings.width,self->settings.height);
  gtk_paned_set_position( GTK_PANED(vpaned),-1);

  gtk_widget_grab_focus (self->user_entry);

#ifdef NOTIFY
  notify_init(APPNAME);
#endif

  self->users_without_avatar = NULL;

  self->session = soup_session_async_new ();
  soup_session_add_feature (self->session, SOUP_SESSION_FEATURE(soup_cookie_jar_new()));
  /*TODO: libxml2 init. */

}

static void
devchat_window_class_init (DevchatWindowClass* klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /*TODO: Simple Attribute wie lastid per g_object_class_install_property installieren.*/

  gobject_class->dispose = devchat_window_dispose;
  gobject_class->finalize = devchat_window_finalize;

}

static void
devchat_window_dispose (GObject* gobject)
{
  DevchatWindow* self = DEVCHAT_WINDOW (gobject);

  /*TODO: Deref all members.*/

  /* Chain up to the parent class */
  G_OBJECT_CLASS (devchat_window_parent_class)->dispose (gobject);
}

static void
devchat_window_finalize (GObject* gobject)
{
  DevchatWindow* self = DEVCHAT_WINDOW (gobject);

  /*TODO: g_free all vars, soup/alsa/notify/etc. uninit. */

  /* Chain up to the parent class */
  G_OBJECT_CLASS (devchat_window_parent_class)->finalize (gobject);
}

/*XXX: Guard all function calls with g_return_if_fail (DEVCHAT_IS_WINDOW (self)); */

void destroy (GtkWidget* widget, DevchatCBData* data)
{
#ifdef NOTIFY
  notify_uninit ();
#endif
  soup_session_abort (data->window->session);
  /*TODO: libxml2 uninit*/
  gtk_main_quit ();
}

void login (GtkWidget* widget, DevchatCBData* data)
{
  if ( !(DEVCHAT_IS_CB_DATA (data)))
    g_print("You're doing it wrong.");
  dbg ("Logging in...");
  data->window->settings.user = g_strdup (gtk_entry_get_text(GTK_ENTRY(data->window->user_entry)));
  data->window->settings.pass = g_strdup (gtk_entry_get_text(GTK_ENTRY(data->window->pass_entry)));
  dbg ("Sup?");
  SoupMessage* loginparams = soup_form_request_new("POST", "http://forum.egosoft.com/login.php","username",data->window->settings.user,"password",data->window->settings.pass,"autologin","on","redirect","","webroot","0","login","Log in",NULL);
  soup_session_queue_message (data->window->session, loginparams, SOUP_SESSION_CALLBACK (login_cb), data);
}

void login_cb (SoupSession* session, SoupMessage* msg, DevchatCBData* data)
{
  dbg ("Got login response from server.");
  if ( g_strrstr(msg->response_body->data,"invalid password"))
  {
    err ("Login failed."); /*TODO: Fallback-Server, Fehlerdialog.*/
  }
  else
  {
    dbg ("This was a triumph.");
    gtk_widget_set_sensitive(data->window->btn_connect,FALSE);
    SoupMessage* step2 = soup_message_new("GET","http://www.egosoft.com");
    dbg ("Trying to determine userlevel...");
    soup_session_queue_message (data->window->session, step2, SOUP_SESSION_CALLBACK(remote_level), data);
  }
}



void remote_level (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  if (g_strrstr (m->response_body->data,"User-Level: 5"))
  {
    data->window->userlevel = 5;
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 1");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 3/DevNet");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 5/Betatest");
    gtk_combo_box_set_active ( GTK_COMBO_BOX(data->window->level_box), 0);
    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
    gtk_widget_show (data->window->item_l5);
  }
  else if (g_strrstr (m->response_body->data,"User-Level: 3"))
  {
    data->window->userlevel = 3;
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 1");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 3/DevNet");
    gtk_combo_box_set_active ( GTK_COMBO_BOX(data->window->level_box), 0);
    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
  }
  else if (g_strrstr (m->response_body->data,"User-Level: "))
  {
    data->window->userlevel = 6;
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 1");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 3");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 5");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 6");
    gtk_combo_box_set_active ( GTK_COMBO_BOX(data->window->level_box), 0);
    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
    gtk_widget_show (data->window->item_l5);
  }
  else
  {
    data->window->userlevel = 1;
  }

  dbg (g_strdup_printf("Determined userlevel to be %i.", data->window->userlevel));

  g_signal_connect(data->window->window, "key-press-event", G_CALLBACK (hotkey_cb), data);
  gtk_widget_grab_focus(data->window->inputwidget);
  gtk_widget_hide_all (data->window->loginbar);
  gtk_widget_show_all (data->window->inputbar);
  gtk_widget_hide (data->window->item_connect);
  gtk_widget_show (data->window->item_reconnect);
  dbg ("Starting requests...");
  SoupMessage* listusers = soup_message_new("GET","http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?users=1");
  soup_session_queue_message (data->window->session, listusers, SOUP_SESSION_CALLBACK (user_list_get), data);
  SoupMessage* listmessages = soup_message_new("GET",g_strdup_printf("http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?lid=%i",data->window->lastid));
  soup_session_queue_message (data->window->session, listmessages, SOUP_SESSION_CALLBACK (message_list_get), data);
}

void user_list_get (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  gchar* userlist = g_strdup (m->response_body->data);
  if (userlist)
  {
    dbg ("Got non-empty userlist.");
    gtk_label_set_text (GTK_LABEL (data->window->statuslabel), g_strdup_printf("Last Update: %s",current_time()));
    xmlTextReaderPtr userparser = xmlReaderForMemory (userlist,strlen(userlist),"",NULL,(XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_NONET));
    int ret;
    while (xmlTextReaderRead (userparser) > 0)
    {
      gchar* node = xmlTextReaderLocalName(userparser);
      guint usercount = 0;
      if (node && (g_strcmp0 (node,"cu") == 0))
      {
        usercount++;
        gchar* name = xmlTextReaderGetAttribute(userparser,"n");
        gchar* uid = xmlTextReaderGetAttribute(userparser,"uid");
        gchar* level = xmlTextReaderGetAttribute(userparser,"l");
        gchar* status = xmlTextReaderGetAttribute(userparser,"s");

        if ( (!g_slist_find(data->window->users_without_avatar,uid)) && (!g_file_test (g_build_filename (data->window->avadir, uid, NULL), G_FILE_TEST_EXISTS)))
        {
          /*TODO: Avatare suchen.*/
          data->window->users_without_avatar = g_slist_prepend(data->window->users_without_avatar,uid);
        }

        /*TODO: Hashtable für n->uid-Zuordnung füllen, falls nötig. */

        if ((g_strcmp0("Away: STEALTH",status) != 0) || (data->window->settings.showhidden))
        {
          dbg (g_strdup_printf("Adding user %s.",name));

          GtkWidget* label = gtk_label_new(NULL);
          GtkWidget* container = gtk_hbox_new(FALSE,0);
          GtkWidget* at_btn = gtk_button_new();
          GtkWidget* profile_btn = gtk_button_new();
          GtkWidget* pm_btn = gtk_button_new();

          gulong real_level = strtoll(g_strndup(level,1),NULL,10);
          gchar* color;
          gchar* style;

          if (real_level > 5)
            color = data->window->settings.color_greens;
          else
            color = data->window->settings.color_blues;

          if ( g_strcmp0("",status) != 0)
          {
            style = "italic";
            gtk_widget_set_has_tooltip(label, TRUE);
            /*TODO: status dekodieren.*/
            gtk_widget_set_tooltip_text(at_btn, status);
          }
          else
          {
            style = "normal";
            gtk_widget_set_tooltip_text(at_btn, g_strdup_printf ("Poke %s",name));
          }
          gchar* markup = g_markup_printf_escaped ("<span foreground='%s' style='%s'>%s</span> <span foreground='%s'>(%s)</span>",color,style,name,data->window->settings.color_font,level);
          gtk_label_set_markup (GTK_LABEL (label),markup);
          g_free (markup);

          /*g_signal_connect (at_btn, "clicked", G_CALLBACK (at_cb),*/

          gtk_box_pack_start (GTK_BOX (container),label,TRUE,TRUE,0);

          gtk_box_pack_start (GTK_BOX (data->window->userlist),container,FALSE,FALSE,0);

          /*TODO: Buttons. */
        }
        /*g_free (name);
        g_free (uid);
        g_free (level);
        g_free (status);*/
      }
      //g_free (node);
    }

    gtk_widget_show_all (data->window->userlist);

    xmlFreeTextReader (userparser);
    g_free(userlist);
  }
}

void message_list_get (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
/*TODO: Nachrichten parsen. */
/*XXX: visited-Attribut von URL-Tags per g_object_set/get_data */
}

void hotkey_cb (GtkWidget* w, DevchatCBData* data)
{
}

void config_cb(GtkWidget* widget, DevchatCBData* data)
{
}

void go_forum(GtkWidget* widget, DevchatCBData* data)
{
}

void go_forump(GtkWidget* widget, DevchatCBData* data)
{
}

void go_forum3(GtkWidget* widget, DevchatCBData* data)
{
}

void go_forum5(GtkWidget* widget, DevchatCBData* data)
{
}

void close_tab(GtkWidget* widget, DevchatCBData* data)
{
}

void reconnect(GtkWidget* widget, DevchatCBData* data)
{
}

void tab_changed(GtkWidget* widget, DevchatCBData* data)
{
}

void tab_changed_win(GtkWidget* widget, DevchatCBData* data)
{
}

void on_motion(GtkWidget* widget, DevchatCBData* data)
{
}

void on_mark_set(GtkWidget* widget, DevchatCBData* data)
{
}

void level_changed (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void btn_send (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void btn_format (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void next_tab (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}
void prev_tab (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}
void show_his (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}
void about_cb (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void notify(gchar* title, gchar* body, GdkPixbuf* icon, DevchatCBData* data)
{
  if (g_strcmp0(data->window->settings.vnotify,"<native>") == 0)
  {
#ifdef NOTIFY
    NotifyNotification* note = notify_notification_new(title,body,NULL,NULL);
    notify_notification_set_icon_from_pixbuf(note,icon);
    notify_notification_add_action(note, "0","Show",NOTIFY_ACTION_CALLBACK (notify_cb),data,NULL);
#else
    err("libnotify support disabled at compile time.");
    data->window->settings.vnotify = g_strdup("<none>");
#endif
  }
  else if (g_strcmp0(data->window->settings.vnotify,"<none>") != 0)
  {
    if (!g_spawn_async (NULL, (gchar**) g_strdup(data->window->settings.vnotify),NULL,G_SPAWN_SEARCH_PATH,NULL,NULL,NULL,NULL))
    {
      err("Failed to launch visual notification process.");
      data->window->settings.vnotify = g_strdup("<none>");
    }
  }

  if (g_strcmp0(data->window->settings.notify,"<native>") == 0)
  {
#ifdef G_OS_UNIX
  /*TODO: ALSA*/
#else
  #ifdef G_OS_WIN32
  /*TODO: DSound*/
  #endif
#endif
  }
  else if (g_strcmp0(data->window->settings.notify,"<none>") != 0)
  {
    if (!g_spawn_async (NULL, (gchar**) g_strdup(data->window->settings.notify),NULL,G_SPAWN_SEARCH_PATH,NULL,NULL,NULL,NULL))
    {
      err("Failed to launch audio notification process.");
      data->window->settings.notify = g_strdup("<none>");
    }
  }
}

void notify_cb(NotifyNotification* note, gchar* action, DevchatCBData* data)
{
  gtk_window_present(GTK_WINDOW(data->window->window));
}

gchar* current_time ()
{
  /*TODO*/
  return g_strdup("");
}
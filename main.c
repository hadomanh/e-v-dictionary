#include <gtk/gtk.h>
#include<stdio.h>				
#include <stdlib.h>
#include <string.h>
#include "inc/btree.h"
#include "jrb.h"
#include <gdk/gdkkeysyms.h>

const gchar *a, *b;

GtkWidget *window;
GtkWidget *search_textView, *search_entry, *search_window;
GtkWidget *view1, *view2, *add_window;
GtkWidget *del_entry, *del_window;

GtkListStore *list;

BTA *book = NULL;

static char code[128] = { 0 };

/* ===== FUNCTION LIST ===== */

// tạo hiệu ứng
void myCSS();

// giao diện các chức năng
void search_interface(GtkWidget widget, gpointer window);
void add_interface(GtkWidget widget, gpointer window);
void del_interface(GtkWidget widget, gpointer window);
void about_interface(GtkWidget *widget, gpointer window);

// xử lý các chức năng
void search_func(GtkWidget *w, gpointer data);
void search_entry_func(GtkWidget *entry, GdkEvent *event);
void suggest_func(char * word, gboolean Tab_pressed);
void edit_func(GtkWidget *w, gpointer data);
void add_func(GtkWidget *w, gpointer data);
void del_func(GtkWidget *w, gpointer data);
void back_func(GtkWidget *widget, gpointer window);
void exit_func(GtkWidget *widget, gpointer window);

// hàm phục vụ search
const char* soundex(const char *s);
int isTheSameSoundExCode(char * soundexlist , char * newword,  char * word, char * soundexWord);
int prefix(const char *big, const char *small);
void show_suggestion(JRB suggestionWords, int number);
void set_textView_text(char * text);

// hiện thông báo
void display_message(GtkWidget *parent, GtkMessageType type, char *mms, char *content);

/* ===== DRIVE CODE ===== */
int main(int argc, char *argv[]){
  btinit();
  book = btopn("data.dat", 0, 1);
  
  //GTK+
  GtkWidget *fixed,*image;
  GtkWidget *btn1,*btn2,*btn3,*btn4,*btn5;
  
  gtk_init(&argc, &argv);
  
  // style.css
  myCSS();
  
  //tao cua so
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "E-V Dictionary");
  gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  
  //tao nen
  fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), fixed);
  
  //anh nen
  image = gtk_image_new_from_file("studying.png");
  gtk_container_add(GTK_CONTAINER(fixed), image);

  // nut chuc nang chinh
  btn1 = gtk_button_new_with_label("Tìm kiếm");// Tìm kiếm
  gtk_widget_set_name(btn1, "search_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn1, 200, 150);
  gtk_widget_set_size_request(btn1, 250, 50);
  gtk_widget_set_tooltip_text(btn1, "Search a word");
  g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(search_interface), NULL);
  
  btn2 = gtk_button_new_with_label("Thêm từ");// Thêm từ
  gtk_widget_set_name(btn2, "add_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn2, 200, 210);
  gtk_widget_set_size_request(btn2, 120, 50);
  gtk_widget_set_tooltip_text(btn2, "Add a word");
  g_signal_connect(G_OBJECT(btn2), "clicked", G_CALLBACK(add_interface), NULL);
  
  btn3 = gtk_button_new_with_label("Xóa từ");// Xóa từ
  gtk_widget_set_name(btn3, "remove_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn3, 330, 210);
  gtk_widget_set_size_request(btn3, 120, 50);
  gtk_widget_set_tooltip_text(btn3, "Delete a word");
  g_signal_connect(G_OBJECT(btn3), "clicked", G_CALLBACK(del_interface), NULL);
  
  btn4 = gtk_button_new_with_label("About");// Thông tin
  gtk_widget_set_name(btn4, "about_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn4, 20, 400);
  gtk_widget_set_size_request(btn4, 60, 50);
  g_signal_connect(G_OBJECT(btn4), "clicked", G_CALLBACK(about_interface), window);
  
  btn5 = gtk_button_new_with_label("Thoát");// Thoát
  gtk_widget_set_name(btn5, "exit_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn5, 560, 400);
  gtk_widget_set_size_request(btn5, 60, 50);
  gtk_widget_set_tooltip_text(btn5, "Exit app");
  g_signal_connect(G_OBJECT(btn5), "clicked", G_CALLBACK(exit_func), window);
  
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(window);
  
  gtk_main();
  
  btcls(book);
  
  return 0;
}



void myCSS(){
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;
  
  provider = gtk_css_provider_new ();
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  
  const gchar *myCssFile = "style.css";
  GError *error = 0;
  
  gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
  g_object_unref (provider);
}



void search_interface(GtkWidget widget, gpointer window){
  GtkWidget *fixed;
  GtkWidget *label;
  GtkWidget *btn1,*btn2,*btn3;
  
  search_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(search_window), "Tra từ");
  gtk_window_set_default_size(GTK_WINDOW(search_window), 750, 400);
  gtk_window_set_position(GTK_WINDOW(search_window), GTK_WIN_POS_CENTER);
  
  fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(search_window), fixed);
  
  label = gtk_label_new("Nhập:");
  gtk_fixed_put(GTK_FIXED(fixed), label, 15, 20);
  
  search_entry = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Nhập từ tiếng Anh...");
  gtk_widget_set_size_request(search_entry, 300, 30);
  gtk_entry_set_max_length(GTK_ENTRY(search_entry),100);
  gtk_fixed_put(GTK_FIXED(fixed), search_entry, 100, 15);
  
  GtkEntryCompletion *comple = gtk_entry_completion_new();
  gtk_entry_completion_set_text_column(GTK_ENTRY_COMPLETION(comple), 0);
  list = gtk_list_store_new(10, G_TYPE_STRING, G_TYPE_STRING,
			    G_TYPE_STRING, G_TYPE_STRING,
			    G_TYPE_STRING, G_TYPE_STRING,
			    G_TYPE_STRING, G_TYPE_STRING,
			    G_TYPE_STRING, G_TYPE_STRING);
  gtk_entry_completion_set_model(GTK_ENTRY_COMPLETION(comple), GTK_TREE_MODEL(list));
  gtk_entry_set_completion(GTK_ENTRY(search_entry), GTK_ENTRY_COMPLETION(comple));
  
  btn1 = gtk_button_new_with_label("Tìm");
  gtk_fixed_put(GTK_FIXED(fixed), btn1, 450, 15);
  gtk_widget_set_size_request(btn1, 80, 30);
  
  label = gtk_label_new("Nghĩa:");
  gtk_fixed_put(GTK_FIXED(fixed), label, 15, 120);
  
  search_textView = gtk_text_view_new();
  gtk_widget_set_size_request(search_textView, 300, 300);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(search_textView), GTK_WRAP_WORD);
  
  GtkWidget *scrolling = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scrolling), search_textView);
  gtk_widget_set_size_request(scrolling, 300, 300);
  gtk_fixed_put(GTK_FIXED(fixed), scrolling, 100, 80);
  
  btn2 = gtk_button_new_with_label("Trở về");
  gtk_fixed_put(GTK_FIXED(fixed), btn2, 550, 15);
  gtk_widget_set_size_request(btn2, 80, 30);
  
  btn3 = gtk_button_new_with_label("Cập nhật nghĩa");
  gtk_fixed_put(GTK_FIXED(fixed), btn3, 450, 100);
  gtk_widget_set_size_request(btn3, 180, 30);
  
  label = gtk_label_new("Lưu ý: Tìm từ trước khi\n chỉnh sửa nghĩa của từ\n ở cửa sổ bên dưới");
  gtk_fixed_put(GTK_FIXED(fixed), label, 450, 150);

  
  g_signal_connect(search_entry, "key-press-event", G_CALLBACK(search_entry_func), NULL);
  
  g_signal_connect(G_OBJECT(search_entry), "activate", G_CALLBACK(search_func), NULL);
  
  g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(search_func), NULL);
  
  g_signal_connect(G_OBJECT(btn2), "clicked", G_CALLBACK(back_func), search_window);
  
  g_signal_connect(G_OBJECT(btn3), "clicked", G_CALLBACK(edit_func), NULL);
  
  g_signal_connect(G_OBJECT(search_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(search_window);
  
  gtk_main();
  
  return;
  
}

void add_interface(GtkWidget widget, gpointer window) {
  GtkWidget *fixed;
  GtkWidget *label;
  GtkWidget *btn1, *btn2;
  
  add_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(add_window), "Thêm từ");
  gtk_window_set_default_size(GTK_WINDOW(add_window), 700, 400);
  gtk_window_set_position(GTK_WINDOW(add_window), GTK_WIN_POS_CENTER);
  
  fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(add_window), fixed);
  
  label = gtk_label_new("Từ:");
  gtk_fixed_put(GTK_FIXED(fixed), label, 15, 20);

  view1 = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(view1), "Nhập từ tiếng Anh...");
  gtk_widget_set_size_request(view1, 300, 30);
  gtk_entry_set_max_length(GTK_ENTRY(view1),100);
  gtk_fixed_put(GTK_FIXED(fixed), view1, 100, 15);
  
  
  label = gtk_label_new("Nghĩa:");
  gtk_fixed_put(GTK_FIXED(fixed), label, 15, 120);
  
  view2 = gtk_text_view_new();
  gtk_widget_set_size_request(view2, 300, 300);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view2), GTK_WRAP_WORD);
  
  GtkWidget *scrolling = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scrolling), view2);
  gtk_widget_set_size_request(scrolling, 300, 300);
  gtk_fixed_put(GTK_FIXED(fixed), scrolling, 100, 80);
  
  btn1 = gtk_button_new_with_label("Thêm");
  gtk_fixed_put(GTK_FIXED(fixed), btn1, 450, 15);
  gtk_widget_set_size_request(btn1, 80, 30);
  
  g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(add_func), NULL);
  
  btn2 = gtk_button_new_with_label("Trở về");
  gtk_fixed_put(GTK_FIXED(fixed), btn2, 550, 15);
  gtk_widget_set_size_request(btn2, 80, 30);
  
  g_signal_connect(G_OBJECT(btn2), "clicked", G_CALLBACK(back_func), add_window);
  
  g_signal_connect(G_OBJECT(add_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(add_window);
  
  gtk_main();
  
  return;
  
}

void del_interface(GtkWidget widget, gpointer window) {
  GtkWidget *table,*fixed;
  GtkWidget *btn1,*label,*btn2;
  
  del_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(del_window), "Xoá từ");
  gtk_window_set_default_size(GTK_WINDOW(del_window), 620, 100);
  gtk_window_set_position(GTK_WINDOW(del_window), GTK_WIN_POS_CENTER);
  
  fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(del_window), fixed);
  
  label = gtk_label_new("Nhập:");
  gtk_fixed_put(GTK_FIXED(fixed), label, 20, 25);
  
  del_entry = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(del_entry), "Nhập từ tiếng Anh...");
  gtk_widget_set_size_request(del_entry, 300, 30);
  gtk_entry_set_max_length(GTK_ENTRY(del_entry),100);
  gtk_fixed_put(GTK_FIXED(fixed), del_entry, 80, 20);
  
  btn1 = gtk_button_new_with_label("Xoá");
  gtk_fixed_put(GTK_FIXED(fixed), btn1, 390, 20);
  gtk_widget_set_size_request(btn1, 80, 30);
  
  btn2 = gtk_button_new_with_label("Trở về");
  gtk_fixed_put(GTK_FIXED(fixed), btn2, 470, 20);
  gtk_widget_set_size_request(btn2, 80, 30);

  
  g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(del_func), NULL);
  
  g_signal_connect(G_OBJECT(del_entry), "activate", G_CALLBACK(del_func), NULL);
  
  g_signal_connect(G_OBJECT(btn2), "clicked", G_CALLBACK(back_func), del_window);
  
  g_signal_connect(G_OBJECT(del_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(del_window);
  
  gtk_main();
  
  return;
  
}

void about_interface(GtkWidget *widget, gpointer window) {
  GtkWidget *about_dialog;
  about_dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "E-V Dictionary");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), " - ver 2018.2 -");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "Do Manh Ha\nHanoi University of Science and Technology\n");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog),
			       "http://m.me/hadomanh99");
  gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about_dialog), "Feedback with us\n");     
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "(c) Copyright 2019 - HEDSPI");
  gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog), NULL);
  gtk_window_set_transient_for(GTK_WINDOW(about_dialog), window);
  gtk_dialog_run(GTK_DIALOG(about_dialog));
  gtk_widget_destroy(about_dialog);
}



void search_func(GtkWidget *w, gpointer data) { // hàm chức năng search
  char word[50], mean[100000];
  int size;
  
  strcpy(word, gtk_entry_get_text(GTK_ENTRY(search_entry)));
  if (word[0] == '\0')
    display_message(GTK_WIDGET(search_window), GTK_MESSAGE_WARNING, "Cảnh báo!", "Cần nhập từ để tra.");
  else{
    if(btsel(book,word,mean,100000,&size)==0){
      set_textView_text(mean);
      return;
    }
    else
      display_message(GTK_WIDGET(search_window),GTK_MESSAGE_ERROR, "Xảy ra lỗi!","Không tìm thấy từ này trong từ điển.");
  }
  return;
}

void search_entry_func(GtkWidget * entry, GdkEvent * event) {// hàm xử lý chuỗi nhập vào
  GdkEventKey *keyEvent = (GdkEventKey *)event;
  char word[50];
  int len;
  strcpy(word, gtk_entry_get_text(GTK_ENTRY(search_entry)));
  
  /* Nếu người dùng sử dụng TAB */
  if (keyEvent->keyval == GDK_KEY_Tab) {
    g_print("TAB pressed\n");
    suggest_func(word,  TRUE);
  }
  else {
	  /* Nếu người dùng xóa từ với BACKSPACE*/
    if (keyEvent->keyval == GDK_KEY_BackSpace) {
		len = strlen(word);
		word[len - 1] = '\0';
    }
    /* Nếu người dùng nhập chữ thông thường*/
    else {
		len = strlen(word);
		word[len] = keyEvent->keyval;
		word[len + 1] = '\0';
    }
    suggest_func(word, FALSE);
  }
}

void edit_func( GtkWidget *w, gpointer data ) { // hàm chức năng edit
  BTint x;
  
  if (gtk_entry_get_text(GTK_ENTRY(search_entry))[0] == 0){
    display_message(search_window, GTK_MESSAGE_INFO, "Cách dùng:", "Phải tìm kiếm trước khi sửa.");
    return;
  }
  if (bfndky(book, (char*)gtk_entry_get_text(GTK_ENTRY(search_entry)), &x) != 0){
    display_message(search_window, GTK_MESSAGE_ERROR, "Không thể cập nhật nghĩa", "Từ bạn nhập chưa có trong từ điển");
    return;
  }
  
  char word[50],mean[100000];
  strcpy(word, gtk_entry_get_text(GTK_ENTRY(search_entry)));
  
  GtkTextBuffer *buffer2;
  GtkTextIter start, end,iter;
  
  buffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(GTK_TEXT_VIEW(search_textView)));
  gtk_text_buffer_get_iter_at_offset(buffer2, &iter, 0);
  
  gtk_text_buffer_insert(buffer2, &iter, "", -1);
  gtk_text_buffer_get_bounds (buffer2, &start, &end);
  b = gtk_text_buffer_get_text (buffer2, &start, &end, FALSE);
  
  strcpy(mean,b);
  
  if (word[0] == '\0' || mean[0] == '\0')
    display_message(GTK_WIDGET(search_window), GTK_MESSAGE_WARNING, "Cảnh báo!", "Không được bỏ trống phần nào.");
  else if (bfndky(book, word, &x ) != 0)
    display_message(GTK_WIDGET(search_window), GTK_MESSAGE_ERROR, "Xảy ra lỗi!", "Từ vừa nhập không có trong từ điển.");
  else{
    if( btupd(book, word, mean, strlen(mean) + 1)==1)
      display_message(GTK_WIDGET(search_window),GTK_MESSAGE_ERROR, "Xảy ra lỗi!","Không thể cập nhật từ.");
    else
      display_message(GTK_WIDGET(search_window),GTK_MESSAGE_INFO, "Thành công!","Đã cập nhật từ.");
  }
  
}

void add_func(GtkWidget *w, gpointer data) { // hàm chức năng add
  GtkTextBuffer *buffer1,*buffer2;
  GtkTextIter start, end,iter;
  char mean[10000], word[50];
  b = gtk_entry_get_text(GTK_ENTRY(view1));
  
  strcpy(word,b);
  /* g_print("%s\n", word); */
  
  buffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(GTK_TEXT_VIEW(view2)));
  gtk_text_buffer_get_iter_at_offset(buffer2, &iter, 0);
  
  gtk_text_buffer_insert(buffer2, &iter, "", -1);
  gtk_text_buffer_get_bounds (buffer2, &start, &end);
  b = gtk_text_buffer_get_text (buffer2, &start, &end, FALSE);
  
  strcpy(mean,b);
  printf("\n");
  /* g_print("%s/n", mean); */
  
  BTint x;
  
  if (word[0] == '\0' || mean[0] == '\0')
    display_message(GTK_WIDGET(data), GTK_MESSAGE_WARNING, "Cảnh báo!", "Không được bỏ trống phần nào.");
  else if (bfndky(book, word, &x ) == 0)
    display_message(GTK_WIDGET(data), GTK_MESSAGE_ERROR, "Xảy ra lỗi!", "Từ vừa nhập đã có trong từ điển.");
  else{
    if(btins(book,word, mean,10000))
      display_message(GTK_WIDGET(data),GTK_MESSAGE_ERROR, "Xảy ra lỗi!","Không thể thêm vào từ điển.");
    else
      display_message(GTK_WIDGET(data),GTK_MESSAGE_INFO, "Thành công!","Đã thêm vào từ điển.");
  }
  
  return;
}

void del_func(GtkWidget *w, gpointer data){ // hàm chức năng delete
  
  a = gtk_entry_get_text(GTK_ENTRY(del_entry));
  /* g_print("%s\n",a); */
  char mean[10000], word[50];
  int size;
  BTint x;
  strcpy(word,a);
  if (word[0] == '\0')
    display_message(GTK_WIDGET(del_window), GTK_MESSAGE_WARNING, "Cảnh báo!", "Cần nhập từ muốn xoá.");
  else if (bfndky(book, word, &x ) != 0)
    display_message(GTK_WIDGET(del_window), GTK_MESSAGE_ERROR, "Xảy ra lỗi!", "Từ vừa nhập không có trong từ điển.");
  else
    if(btsel(book,word,mean,100000,&size)==0)
      {
	btdel(book,word);
	display_message(GTK_WIDGET(del_window),GTK_MESSAGE_INFO, "Thành công!","Đã xoá từ khỏi từ điển.");
	
      }
    else
      display_message(GTK_WIDGET(del_window),GTK_MESSAGE_ERROR, "Xảy ra lỗi!","Không thể xoá từ khỏi từ điển.");
    
  return;
}

void suggest_func(char * word, gboolean Tab_pressed) { // tự động gợi ý từ hoặc TAB để gợi ý
  char  currword[100], nextword[100], prevword[100];
  int i, len, min_len;
  int max;
  GtkTreeIter Iter;
  JRB tmp, suggestionWords = make_jrb(); // tạo cây suggestionWords
  BTint value, existed = 0;
   
  strcpy(currword, word);
  int wordlen = strlen(word);
  gtk_list_store_clear(GTK_LIST_STORE(list));
  
  /* nếu có "word" trong "book" thì thêm vào "list"*/
  if (bfndky(book, word, &value) ==  0) {
    existed = 1;
    gtk_list_store_append(GTK_LIST_STORE(list), &Iter);
    gtk_list_store_set(GTK_LIST_STORE(list), &Iter, 0, word, -1 );
  }
  
  /* nếu chưa có "word" trong "book" */
  if (!existed){
	btins(book, currword, "", 1); // chèn key không có value vào book
  }
  
  for (i = 0; i < 1945; i++) {
    bnxtky(book, currword, &value);  // tìm 'key' tiếp theo
    if (prefix(currword, word)) { // nếu giống prefix
      jrb_insert_str(suggestionWords, strdup(currword), JNULL);  // chèn vào cây jrb "suggestionWords"
    }
    else break;
  }
  
  /* khi người dùng sử dụng TAB để gợi ý từ */
  if (Tab_pressed) { 
	
	/* Nếu cây suggestionWords rỗng*/
    if (jrb_empty(suggestionWords)) {
      g_print("Case 1\n");
      char soundexlist[1000] = "Ý bạn là:\n";
      char soundexWord[50];
      strcpy(nextword, word);
      strcpy(prevword, word);
      strcpy(soundexWord, soundex(word)); // mã hóa "word" bằng soundex, đưa vào "soundexWord"
      
      /* Đưa những từ đồng âm xung quanh vào soundexList */
      max = 5;
      for (i = 0; (i < 10000 ) && max; i++) {
	if (bprvky(book , prevword, &value) == 0)
	  if (isTheSameSoundExCode(soundexlist, prevword, word, soundexWord))
	    max--;
      }
      max = 5;
      for (i = 0; (i < 10000 ) && max; i++) {
	if (bnxtky(book, nextword, &value) == 0)
	  if (isTheSameSoundExCode(soundexlist, nextword, word, soundexWord))
	    max--;
      }
      set_textView_text(soundexlist); // lưu "soundexlist" vào bộ nhớ đệm
    }
    
    /* Nếu cây suggestionWords có từ*/
    else {
      min_len = 1000;
      g_print("Case 2\n");
      jrb_traverse(tmp, suggestionWords) { // duyệt cây, tìm từ ngắn nhất giống với từ mình nhập vào để thay thế
	len = strlen(jval_s(tmp->key));
	if (min_len > len && len != wordlen){
	  min_len = len;
	  strcpy(currword, jval_s(tmp->key));
	  g_print("%s\n", currword);
	  g_print("Min: %d\n", min_len);
	}
      }
      
      if ((min_len  != 1000) && (min_len > wordlen)) { // nếu có từ để thay thế
	currword[min_len] = '\0'; // chọn từ đó
	g_print("Result: %s\n", currword);
	gtk_entry_set_text(GTK_ENTRY(search_entry), currword); // thay thế từ trong thanh tìm kiếm bằng "currword"
	gtk_editable_set_position(GTK_EDITABLE(search_entry), min_len); // cho con trỏ xuống cuối
      }
    }
    
  }
  
  /* Nếu người dùng không TAB*/
  else
    show_suggestion(suggestionWords, i); // tự động gợi ý
  
  /* Tìm mãi không được thì xóa thôi :v */
  if (!existed)
    btdel(book, word); 
  
  /* Xóa cây tạm thời*/
  jrb_free_tree(suggestionWords);
}

// seen
const char* soundex(const char *s) {// mã hóa từ sang soundEx code
  static char out[5];
  int c, prev, i;
  
  out[0] = out[4] = 0;
  if (!s || !*s) return out;
  
  out[0] = *s++;
  
  /* first letter, though not coded, can still affect next letter: Pfister */
  prev = code[(int)out[0]];
  for (i = 1; *s && i < 4; s++) {
    if ((c = code[(int) * s]) == prev) continue;
    
    if (c == -1) prev = 0;	/* vowel as separator */
    else if (c > 0) {
      out[i++] = c + '0';
      prev = c;
    }
  }
  while (i < 4) out[i++] = '0';
  return out;
}

// seen
int isTheSameSoundExCode(char * soundexlist , char * newword,  char * word, char * soundexWord) {// gợi ý từ có cách đọc gần giống(cùng soundEx code)
  if (strcmp(soundexWord, soundex(newword)) == 0) {
    if (strcmp(newword, word) != 0) {
      strcat(soundexlist, newword);
      strcat(soundexlist, "\n");
      return 1;
    }
  }
  else
    return 0;
}

// seen
int prefix(const char *big, const char *small) {// kiểm tra xâu nhỏ có là tiền tố của xâu lớn không
  int small_len = strlen(small);
  int big_len = strlen(big);
  int i;
  if (big_len < small_len)
    return 0;
  for (i = 0; i < small_len; i++)
    if (big[i] != small[i])
      return 0;
  return 1;    
}

// seen
void show_suggestion(JRB suggestionWords, int number) { // sổ ra danh sách từ gợi ý
  GtkTreeIter Iter;
  JRB tmp;
  int max = 8;
  jrb_traverse(tmp, suggestionWords) {
    if ((number--) % 9 == 0)  {
      gtk_list_store_append(GTK_LIST_STORE(list), &Iter);
      gtk_list_store_set(GTK_LIST_STORE(list), &Iter, 0, jval_s(tmp->key), -1 );
      if (max-- < 1)
	return;
    }
  }
}

// seen
void set_textView_text(char * text) {// đưa từ vào bộ nhớ đệm của GTK
  GtkTextBuffer *buffer;
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(search_textView));
  if (buffer == NULL)
    buffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(buffer, text, -1);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(search_textView), buffer);
}

void display_message(GtkWidget *parent, GtkMessageType type, char *mms, char *content) { // dua ra thong bao 
  GtkWidget *mdialog;
  mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),
				   GTK_DIALOG_DESTROY_WITH_PARENT,
				   type,
				   GTK_BUTTONS_OK,
				   "%s", mms);
  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s",  content);
  gtk_dialog_run(GTK_DIALOG(mdialog));
  gtk_widget_destroy(mdialog);
}

void back_func(GtkWidget *widget, gpointer window) {
    gtk_widget_destroy(GTK_WIDGET(window));
}

void exit_func(GtkWidget *widget, gpointer window) {
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_QUESTION,
				  GTK_BUTTONS_YES_NO,
				  "Are you sure to quit?");
  gtk_window_set_title(GTK_WINDOW(dialog), "Quit");
  
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
    gtk_widget_destroy(GTK_WIDGET(window));
  else gtk_widget_destroy(dialog);
}

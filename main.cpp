#include <QDebug>
#include <glib.h>
#include <gio/gio.h>

void show_help() {
  qDebug ()<<"--list-trash: show the file's name in the 'trash'"<<endl
           <<"--trash-file [uri|path]: remove a file to the trash by file's uri, uri is \"file:///xxx\", \n\tpath is like '/xxx/xxx', you can drag a file to terminal as an arg"<<endl
           <<"--restore-file [uri]: restore a file from trash, uri is \"trash:///xxx\", \n\tyou can drag the file in the trash to terminal as an arg"<<endl
           <<"--show-orig [uri]: show a trashed file orig file path, uri is \"trash:///xxx\""<<endl;
}

void list_trash () {
    GFile *trash = g_file_new_for_uri("trash:///");
    GFileEnumerator *enumerator = g_file_enumerate_children(trash,
                                                            G_FILE_ATTRIBUTE_STANDARD_NAME,
                                                            //"standard::*,access::*,mountable::*,time::*,unix::*,owner::*,selinux::*,thumbnail::*,id::filesystem,trash::orig-path,trash::deletion-date,metadata::*",
                                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                            NULL,
                                                            NULL);

    int count = 0;
    GFileInfo *info;
    while ((info = g_file_enumerator_next_file (enumerator, NULL, NULL)) != NULL) {
        //qDebug()<<g_file_info_get_display_name (info);
        qDebug()<<g_file_info_get_name (info);
        g_object_unref(info);
        count++;
    }
    if (count == 0)
        qDebug()<<"trash is empty!";

    g_object_unref(enumerator);
    g_object_unref(trash);
}

char* get_orig_path (char *uri) {
    GFile* file = g_file_new_for_commandline_arg(uri);
    GFileInfo *info = g_file_query_info(file,
                                        G_FILE_ATTRIBUTE_TRASH_ORIG_PATH,
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        NULL,
                                        NULL);

    const char* orig_path = g_file_info_get_attribute_byte_string(info, G_FILE_ATTRIBUTE_TRASH_ORIG_PATH);

    char *real_orig_path = g_strdup (orig_path);
    qDebug()<<orig_path;

    if (orig_path) {
        g_object_unref(info);
    }

    g_object_unref(file);
    return real_orig_path;
}

void trash_file (char *str) {
    qDebug()<<"send "<<str<<" to trash:";
    if (!g_str_has_prefix(str, "file:///")) {
        GFile *file = g_file_new_for_commandline_arg(str);
        if (!g_file_query_exists (file, NULL)) {
            qDebug()<<"file does not exist!";
            return;
        }
        if(!g_file_trash (file, NULL, NULL)) {
            qDebug()<<"failed to trash the file!";
            return;
        }
    } else {
        GFile *file = g_file_new_for_uri(str);
        if(!g_file_trash (file, NULL, NULL)) {
            qDebug()<<"failed to trash the file!";
            return;
        }
    }
}

void restore_file (char *uri) {
    qDebug()<<"restore "<<uri<<" from trash";
    if (!g_str_has_prefix(uri, "trash:///")) {
        qDebug()<<"file must has prefix 'trash:///'";
        return;
    } else {
        GFile *src = g_file_new_for_uri(uri);

        if (!g_file_query_exists (src, NULL)) {
            qDebug()<<"file does not exist!";
            g_object_unref(src);
            return;
        }

        char *orig_path = get_orig_path(uri);
        if (orig_path != NULL) {
            //GFileCopyFlags flags = static_cast<GFileCopyFlags>(G_FILE_COPY_NONE | G_FILE_COPY_OVERWRITE | G_FILE_COPY_ALL_METADATA | G_FILE_COPY_NOFOLLOW_SYMLINKS);
            GFileCopyFlags flags = static_cast<GFileCopyFlags>(G_FILE_COPY_NOFOLLOW_SYMLINKS);
            qDebug()<<"restoring... "<<orig_path;
            GError *err = NULL;
            GFile *dest = g_file_new_for_path(orig_path);

            if (g_file_move(src,
                            dest,
                            flags,
                            NULL,
                            NULL,
                            NULL,
                            &err)) {
                qDebug()<<"restore success!";
                g_object_unref(dest);
                g_object_unref(src);
                g_free (orig_path);
                return;
            } else {
                if (err) {
                    fprintf (stderr, "Unable to restore file: %s\n", err->message);
                    g_error_free (err);
                }
            }
        }
    }
    qDebug()<<"restore err!";
}

int main(int argc, char *argv[])
{
    qDebug()<<"count: "<<argc;
    qDebug()<<argv[1];
    if (argc == 1) {
        qDebug()<<"use help: ./demo -h";
        return 0;
    }

    if (g_str_equal(argv[1], "-h")) {
        show_help();
        return 0;
    }

    if (g_str_equal(argv[1], "--list-trash")) {
        list_trash();
        return 0;
    }

    if (argc != 3) {
        qDebug()<<"please use a correct cmd";
        return 0;
    }

    if (!g_str_has_prefix(argv[1], "-")) {
        qDebug()<<"please use a correct cmd";
    }

    if (g_str_equal(argv[1], "--trash-file")) {
        trash_file (argv[2]);
    } else if (g_str_equal(argv[1], "--restore-file")) {
        restore_file (argv[2]);
    } else if (g_str_equal(argv[1], "--show-orig")) {
        char *orig_path = get_orig_path(argv[2]);
        //qDebug()<<orig_path;
        g_free (orig_path);
    }

    return 0;
}

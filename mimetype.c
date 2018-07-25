#include <string.h>

#define ASCII 0
#define BINARY 1

#define check_type(x, y, z, buf)                                  \
    {                                                             \
        if (strcmp(x, y) == 0) {                                  \
            strcpy(buf, z);                                       \
            return (strncmp(z, "text", 4)) == 0 ? ASCII : BINARY; \
        }                                                         \
    }

int content_type(char *path, char *buf) {
    char *type;
    type = path + strlen(path);
    while ((*type) != '.' && (type > path)) {
        type--;
    }

    /* Audio */
    check_type(type, ".mid", "audio/midi", buf);
    check_type(type, ".midi", "audio/midi", buf);
    check_type(type, ".kar", "audio/midi", buf);
    check_type(type, ".aac", "audio/mp4", buf);
    check_type(type, ".f4a", "audio/mp4", buf);
    check_type(type, ".f4b", "audio/mp4", buf);
    check_type(type, ".m4a", "audio/mp4", buf);
    check_type(type, ".mp3", "audio/mpeg", buf);
    check_type(type, ".oga", "audio/ogg", buf);
    check_type(type, ".ogg", "audio/ogg", buf);
    check_type(type, ".opus", "audio/ogg", buf);
    check_type(type, ".ra", "audio/x-realaudio", buf);
    check_type(type, ".wav", "audio/x-wav", buf);

    /* Images */
    check_type(type, ".bmp", "image/bmp", buf);
    check_type(type, ".gif", "image/gif", buf);
    check_type(type, ".jpg", "image/jpeg", buf);
    check_type(type, ".jpeg", "image/jpeg", buf);
    check_type(type, ".png", "image/png", buf);
    check_type(type, ".svg", "image/svg+xml", buf);
    check_type(type, ".svgz", "image/svg+xml", buf);
    check_type(type, ".tif", "image/tiff", buf);
    check_type(type, ".tiff", "image/tiff", buf);
    check_type(type, ".wbmp", "image/vnd.wap.wbmp", buf);
    check_type(type, ".webp", "image/webp", buf);
    check_type(type, ".ico", "image/x-icon", buf);
    check_type(type, ".cur", "image/x-icon", buf);
    check_type(type, ".jng", "image/x-jng", buf);

    /* JavaScript */
    check_type(type, ".js", "application/javascript", buf);
    check_type(type, ".json", "application/json", buf);

    /* Manifest files */
    check_type(type, ".webapp", "application/x-web-app-manifest+json", buf);
    check_type(type, ".appcache", "text/cache-manifest", buf);
    check_type(type, ".manifest", "text/cache-manifest", buf);

    /* Microsoft Office */
    check_type(type, ".doc", "application/msword", buf);
    check_type(type, ".xls", "application/vnd.ms-excel", buf);
    check_type(type, ".ppt", "application/vnd.ms-powerpoint", buf);
    check_type(type, ".docx",
               "application/"
               "vnd.openxmlformats-officedocument.wordprocessingml.document",
               buf);
    check_type(
        type, "xlsx",
        "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
        buf);
    check_type(type, ".pptx",
               "application/"
               "vnd.openxmlformats-officedocument.presentationml.presentation",
               buf);

    /* Video */
    check_type(type, ".3gpp", "video/3gpp", buf);
    check_type(type, ".3gp", "video/3gpp", buf);
    check_type(type, ".mp4", "video/mp4", buf);
    check_type(type, ".m4v", "video/mp4", buf);
    check_type(type, ".f4v", "video/mp4", buf);
    check_type(type, ".f4p", "video/mp4", buf);
    check_type(type, ".mpeg", "video/mpeg", buf);
    check_type(type, ".mpg", "video/mpeg", buf);
    check_type(type, ".ogv", "video/ogg", buf);
    check_type(type, ".mov", "video/quicktime", buf);
    check_type(type, ".webm", "video/webm", buf);
    check_type(type, ".flv", "video/x-flv", buf);
    check_type(type, ".mng", "video/x-mng", buf);
    check_type(type, ".asx", "video/x-ms-asf", buf);
    check_type(type, ".asf", "video/x-ms-asf", buf);
    check_type(type, ".wmv", "video/x-ms-wmv", buf);
    check_type(type, ".avi", "video/x-msvideo", buf);

    /* Web feeds */
    check_type(type, ".atom", "application/xml", buf);
    check_type(type, ".rdf", "application/xml", buf);
    check_type(type, ".rss", "application/xml", buf);
    check_type(type, ".xml", "application/xml", buf);

    /* Web fonts */
    check_type(type, ".woff", "application/font-woff", buf);
    check_type(type, ".woff2", "application/font-woff2", buf);
    check_type(type, ".eot", "application/vnd.ms-fontobject", buf);
    check_type(type, ".ttc", "application/x-font-ttf", buf);
    check_type(type, ".ttf", "application/x-font-ttf", buf);
    check_type(type, ".otf", "font/opentype", buf);

    /* Other */
    check_type(type, ".jar", "application/java-archive", buf);
    check_type(type, ".war", "application/java-archive", buf);
    check_type(type, ".ear", "application/java-archive", buf);
    check_type(type, ".hqx", "application/mac-binhex40", buf);
    check_type(type, ".pdf", "application/pdf", buf);
    check_type(type, ".ps", "application/postscript", buf);
    check_type(type, ".eps", "application/postscript", buf);
    check_type(type, ".ai", "application/postscript", buf);
    check_type(type, ".rtf", "application/rtf", buf);
    check_type(type, ".wmlc", "application/vnd.wap.wmlc", buf);
    check_type(type, ".xhtml", "application/xhtml+xml", buf);
    check_type(type, ".kml", "application/vnd.google-earth.kml+xml", buf);
    check_type(type, ".kmz", "application/vnd.google-earth.kmz", buf);
    check_type(type, ".7z", "application/x-7z-compressed", buf);
    check_type(type, ".crx", "application/x-chrome-extension", buf);
    check_type(type, ".oex", "application/x-opera-extension", buf);
    check_type(type, ".xpi", "application/x-xpinstall", buf);
    check_type(type, ".cco", "application/x-cocoa", buf);
    check_type(type, ".jardiff", "application/x-java-archive-diff", buf);
    check_type(type, ".jnlp", "application/x-java-jnlp-file", buf);
    check_type(type, ".run", "application/x-makeself", buf);
    check_type(type, ".pl", "application/x-perl", buf);
    check_type(type, ".pm", "application/x-perl", buf);
    check_type(type, ".prc", "application/x-pilot", buf);
    check_type(type, ".pdb", "application/x-pilot", buf);
    check_type(type, ".rar", "application/x-rar-compressed", buf);
    check_type(type, ".rpm", "application/x-redhat-package-manager", buf);
    check_type(type, ".sea", "application/x-sea", buf);
    check_type(type, ".swf", "application/x-shockwave-flash", buf);
    check_type(type, ".sit", "application/x-stuffit", buf);
    check_type(type, ".tcl", "application/x-tcl", buf);
    check_type(type, ".tk", "application/x-tcl", buf);
    check_type(type, ".der", "application/x-x509-ca-cert", buf);
    check_type(type, ".pem", "application/x-x509-ca-cert", buf);
    check_type(type, ".crt", "application/x-x509-ca-cert", buf);
    check_type(type, ".torrent", "application/x-bittorrent", buf);
    check_type(type, ".zip", "application/zip", buf);

    check_type(type, ".bin", "application/octet-stream", buf);
    check_type(type, ".exe", "application/octet-stream", buf);
    check_type(type, ".dll", "application/octet-stream", buf);
    check_type(type, ".deb", "application/octet-stream", buf);
    check_type(type, ".dmg", "application/octet-stream", buf);
    check_type(type, ".iso", "application/octet-stream", buf);
    check_type(type, ".img", "application/octet-stream", buf);
    check_type(type, ".msi", "application/octet-stream", buf);
    check_type(type, ".msp", "application/octet-stream", buf);
    check_type(type, ".msm", "application/octet-stream", buf);
    check_type(type, ".safariextz", "application/octet-stream", buf);
    check_type(type, ".*", "application/octet-stream", buf);

    check_type(type, ".css", "text/css", buf);
    check_type(type, ".html", "text/html", buf);
    check_type(type, ".htm", "text/html", buf);
    check_type(type, ".shtml", "text/html", buf);
    check_type(type, ".mml", "text/mathml", buf);
    check_type(type, ".txt", "text/plain", buf);
    check_type(type, ".jad", "text/vnd.sun.j2me.app-descriptor", buf);
    check_type(type, ".wml", "text/vnd.wap.wml", buf);
    check_type(type, ".vtt", "text/vtt", buf);
    check_type(type, ".htc", "text/x-component", buf);
    check_type(type, ".vcf", "text/x-vcard", buf);

    strcpy(buf, "application/octet-stream");
    return BINARY;
}

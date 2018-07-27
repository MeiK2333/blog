#!/usr/bin/env python3
import cgi
import os
import sys
import time

template = '''
<!doctype html>
<html>
<head>
    <title>Index</title>
    <meta content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0" name="viewport"/>
    <link rel="stylesheet" href="/static/css/github-light.css">
    <link rel="stylesheet" href="/static/css/github.css">
    <link rel="stylesheet" href="/static/css/blog.css">
    <script crossorigin="anonymous" type="application/javascript" src="/static/js/frameworks.js"></script>
    <script crossorigin="anonymous" async="async" type="application/javascript" src="/static/js/github.js"></script>
</head>
<body>
    <div class="markdown-body blog-container">
        <table>
            <thead>
                <tr>
                <th>Title</th>
                    <th>Created At</th>
                    <th>Last Modify</th>
                </tr>
            </thead>
            <tbody>
        {body}
            </tbody>
        </table>
    </div>
</body>
</html>

'''


class Blog(object):
    def __init__(self, stat, title):
        self.stat = stat
        self.title = title


if __name__ == '__main__':
    path = sys.argv[1]
    file_stat_list = sorted([Blog(os.stat(os.path.join(path, x)), x[:-5]) for x in os.listdir(
        path) if x.endswith(".html")], key=lambda x: x.stat.st_ctime, reverse=True)
    body = []
    for f in file_stat_list:
        body.append('<tr><td><a href="{href}">{title}</a></td><td>{c_time}</td><td>{m_time}</td></tr>\n'.format(
            href=cgi.escape(f.title + ".html"), title=cgi.escape(f.title), c_time=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(f.stat.st_ctime)), m_time=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(f.stat.st_mtime))))
    with open(os.path.join(path, 'index.html'), 'w') as fw:
        fw.write(template.format(body=''.join(body)))

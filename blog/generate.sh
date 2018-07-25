#!/bin/bash
cd blog
rm *.html

# 请求 GitHub API 渲染 markdown
for file in *.md
do
    value=$(<"${file}")
    filename=${file%.md}
    echo ${filename}
    curl https://api.github.com/markdown/raw -X "POST" -H "Content-Type: text/plain"  -d "${value}" -o "${filename}".html -#
done

# 添加自定义的 HTML
for file in *.md
do
    filename=${file%.md}
    value=$(<"${filename}".html)
    echo "<!doctype html>
<html>
<head>
    <title>${filename}</title>
    <link rel=\"stylesheet\" href=\"/static/css/github-light.css\">
    <link rel=\"stylesheet\" href=\"/static/css/github.css\">
    <script src=\"/static/js/github.js\"></script>
</head>
<body>
    <div class=\"markdown-body\">
${value}
    </div>
</body>
</html>" > "${filename}".html
done
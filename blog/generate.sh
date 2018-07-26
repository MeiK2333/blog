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
    <meta content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\" name=\"viewport\"/>
    <link rel=\"stylesheet\" href=\"/static/css/github-light.css\">
    <link rel=\"stylesheet\" href=\"/static/css/github.css\">
    <link rel=\"stylesheet\" href=\"/static/css/blog.css\">
    <script crossorigin=\"anonymous\" type=\"application/javascript\" src=\"/static/js/frameworks.js\"></script>
    <script crossorigin=\"anonymous\" async=\"async\" type=\"application/javascript\" src=\"/static/js/github.js\"></script>
</head>
<body>
    <div class=\"markdown-body blog-container\">
${value}
    </div>
</body>
</html>" > "${filename}".html
done
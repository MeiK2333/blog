#!/bin/bash
cd blog
rm *.html
for file in *.md
do
    value=$(<"${file}")
    filename=${file%.md}
    echo ${filename}
    curl https://api.github.com/markdown/raw -X "POST" -H "Content-Type: text/plain"  -d "${value}" -o "${filename}".html -#
done

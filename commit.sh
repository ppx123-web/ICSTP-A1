#! /bin/zsh

git add .
git commit -am "$*"
git push origin $1

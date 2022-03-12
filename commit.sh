#! /bin/zsh

git fetch
git pull origin $1
git add .
git commit -am "$*  $(date)"
git push origin $1

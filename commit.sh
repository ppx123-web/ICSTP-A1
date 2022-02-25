#! /bin/zsh

git fetch
git pull origin
git add .
git commit -am "$(date)"
git push origin main

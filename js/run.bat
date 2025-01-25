@echo off
if not exist node_modules (call npm i)
title ZeroFN
node app.js
cmd /k
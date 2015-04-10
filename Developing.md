

# flybot 0.3 #

Для сборки релиз конфигурации требуются

  * Дистрибутив subversion для Windows например, [sliksvn](http://www.sliksvn.com/en/download)

  * [NSIS](http://nsis.sourceforge.net/Download)

  * [poEdit](http://www.poedit.net/download.php) - для редактирования файлов локализации (.po)

Код [здесь](http://flybot.googlecode.com/svn/trunk). Нужная версия wxWidgets скачается с кодом.
  * Накатите на папку wxWidgets
```
    scripts\wxWidgets.2.9.1.vc2010.patch
```

  * Чтобы получить бинарник вменяемого размера переместите содержимое scripts\setup.h в папку wxWidgets\include\msw\.

  * Соберите wxWidgets (проект тут: build/msw/wx.sln).

> Последние три шага может выполнить простой [скрипт](http://code.google.com/p/flybot/source/browse/trunk/scripts/workspace-config.bat) в папке с исходными текстами:
```
  scripts\workspace-setup.bat
```

# flybot 0.2.x (coolbot-based) #

Проект собирался под Delphi 7.

Были мысли писать c нуля, но потом нашелся
Delphi пример из apex speedmod, который и был доработан :).

Для успешной компиляции требуется компонент [CoolTrayIcon](http://flybot.googlecode.com/files/CoolTrayIcon.zip).
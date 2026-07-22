@echo off
REM ElysiumClient - GitHub'a otomatik yukleme / guncelleme
REM Cift tikla: degisiklikleri commitler ve mnknx/ElysiumClient reposuna gonderir.
chcp 65001 >nul
cd /d "%~dp0"

echo ==========================================
echo   ElysiumClient GitHub Yukleme
echo ==========================================
echo.

REM Degisiklikleri sahnele (.gitignore geregi build/, ddnet-libs/, .claude/ haric)
git add -A

REM Sahnelenmis degisiklik var mi?
git diff --cached --quiet
if %errorlevel%==0 (
    echo Yeni degisiklik yok - mevcut durum gonderiliyor...
) else (
    git commit -m "Update: %date% %time%"
    echo Commit olusturuldu.
)

echo.
echo GitHub'a gonderiliyor...
git push -u origin main
if %errorlevel%==0 (
    echo.
    echo [OK] Yuklendi: https://github.com/mnknx/ElysiumClient
) else (
    echo.
    echo [HATA] Gonderilemedi!
    echo  - Fork henuz yoksa once olustur: https://github.com/ddnet/ddnet/fork
    echo    Owner: mnknx  /  Repository name: ElysiumClient
    echo  - Giris penceresi acildiysa GitHub hesabinla giris yap.
)
echo.
pause

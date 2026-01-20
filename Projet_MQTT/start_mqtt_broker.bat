@echo off
echo ========================================
echo   Demarrage du broker MQTT Mosquitto
echo ========================================
echo.

REM Verifier si Docker est lance
docker info >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERREUR] Docker n'est pas lance ou n'est pas installe.
    echo Veuillez demarrer Docker Desktop et reessayer.
    pause
    exit /b 1
)

echo [INFO] Docker est pret.
echo.

REM Arreter et supprimer le conteneur existant s'il existe
echo [INFO] Nettoyage des conteneurs existants...
docker stop mqtt-broker 2>nul
docker rm mqtt-broker 2>nul

REM Supprimer l'ancien script stop s'il existe dans le même dossier
if exist "%~dp0stop_mqtt_broker.bat" (
  echo [INFO] Ancien script stop_mqtt_broker.bat trouve - suppression...
  del /q "%~dp0stop_mqtt_broker.bat"
  if exist "%~dp0stop_mqtt_broker.bat" (
    echo [WARN] Impossible de supprimer stop_mqtt_broker.bat - verifiez les permissions.
  ) else (
    echo [INFO] stop_mqtt_bat supprime.
  )
)

echo.
echo [INFO] Lancement du broker Mosquitto sur le port 1883...
echo.

REM Lancer le conteneur Mosquitto
docker run -d --name mqtt-broker -p 1883:1883 -p 9001:9001 -v "%~dp0mosquitto\config\mosquitto.conf":/mosquitto/config/mosquitto.conf eclipse-mosquitto:latest >nul 2>&1

if %errorlevel% neq 0 (
    echo.
    echo [ERREUR] Echec du lancement du broker.
    echo Verifiez que l'image eclipse-mosquitto est disponible et que Docker fonctionne.
    echo.
    echo Pour telecharger l'image :
    echo   docker pull eclipse-mosquitto:latest
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo   SUCCES - Broker MQTT demarre !
echo ========================================
echo.
echo   Port MQTT : 1883
echo   Port WebSocket : 9001
echo   Nom du conteneur : mqtt-broker
echo.

REM Récupérer l'adresse IPv4 locale (ignorer interfaces virtuelles) et la copier dans le presse-papiers
for /f "usebackq delims=" %%I in (`powershell -NoProfile -Command "$if = Get-NetAdapter | Where-Object { $_.Status -eq 'Up' -and $_.InterfaceDescription -notmatch 'Virtual|vEthernet|Docker|Hyper-V|Loopback' } | Select-Object -First 1; if ($if) { $addr = Get-NetIPAddress -InterfaceIndex $if.ifIndex -AddressFamily IPv4 | Where-Object { $_.IPAddress -notlike '169.254.*' -and $_.IPAddress -ne '127.0.0.1' } | Select-Object -First 1 -ExpandProperty IPAddress } else { $addr = Get-NetIPAddress -AddressFamily IPv4 | Where-Object { $_.IPAddress -notlike '169.254.*' -and $_.IPAddress -ne '127.0.0.1' } | Select-Object -First 1 -ExpandProperty IPAddress }; if ($addr) { Set-Clipboard $addr; Write-Output $addr } else { Write-Output 'IP_INCONNUE' }"`) do set HOST_IP=%%I

echo [INFO] Adresse IP du PC (copie dans le presse-papiers) : %HOST_IP%

echo.

REM Ouvrir une nouvelle fenêtre PowerShell affichant les logs et attendre une touche ici pour arreter
powershell -NoProfile -Command "Start-Process -FilePath 'powershell' -ArgumentList '-NoExit','-Command','docker logs -f mqtt-broker' ; Write-Host 'Appuyez sur une touche pour arreter le broker...'; $null = $Host.UI.RawUI.ReadKey('NoEcho,IncludeKeyDown'); docker stop mqtt-broker | Out-Null; docker rm mqtt-broker | Out-Null; Write-Host 'Broker arrete et supprime.'"

echo.
echo [INFO] Fin du script.
pause

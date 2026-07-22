# ElysiumClient - tek tikla GitHub Release + oyun paketi yukleme
# Kimlik: Git Credential Manager'daki yerel GitHub token'i kullanilir (asla yazdirilmaz).
param([string]$Version)

$ErrorActionPreference = "Stop"
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
$proj = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $proj

# repo bilgisini origin'den al
$originUrl = (git remote get-url origin).Trim()
if($originUrl -notmatch "github\.com[:/]+([^/]+)/([^/.]+)") { Write-Host "[HATA] origin GitHub deposu degil: $originUrl"; exit 1 }
$owner = $Matches[1]; $repo = $Matches[2]

# surum secimi (parametre yoksa sor; bos birakilirsa en son tag)
$latestTag = git tag -l --sort=-creatordate | Select-Object -First 1
if(-not $Version) {
    $inp = $null
    try { $inp = Read-Host "Surum etiketi (bos = $latestTag, yeni icin orn. v0.2.0)" } catch {}
    if($inp) { $Version = $inp } else { $Version = $latestTag }
}
if(-not $Version) { Write-Host "[HATA] Surum belirtilmedi ve mevcut tag yok."; exit 1 }

# tag yoksa olustur, remote'a gonder
$tagExists = git tag -l $Version
if(-not $tagExists) {
    Write-Host "Yeni tag olusturuluyor: $Version"
    git tag -a $Version -m "ElysiumClient $Version"
}
git push origin $Version --quiet
if($LASTEXITCODE -ne 0) { Write-Host "[HATA] Tag gonderilemedi."; exit 1 }

# oyun paketini hazirla (build gereklidir)
$exe = Join-Path $proj "build\DDNet.exe"
if(-not (Test-Path $exe)) { Write-Host "[HATA] build\DDNet.exe yok. Once derle (build_elysium.bat)."; exit 1 }
$zipName = "ElysiumClient-$Version-win64.zip"
$zip = Join-Path $proj $zipName
$stageRoot = Join-Path $env:TEMP "ElysiumRelStage"
$stage = Join-Path $stageRoot "ElysiumClient"
if(Test-Path $stageRoot) { Remove-Item -Recurse -Force $stageRoot }
New-Item -ItemType Directory -Force $stage | Out-Null
Copy-Item $exe $stage
Copy-Item (Join-Path $proj "build\*.dll") $stage
Copy-Item -Recurse (Join-Path $proj "build\data") (Join-Path $stage "data")
Copy-Item (Join-Path $proj "license.txt") $stage
Copy-Item (Join-Path $proj "storage.cfg") $stage
if(Test-Path $zip) { Remove-Item -Force $zip }
Write-Host "Paketleniyor: $zipName ..."
Compress-Archive -Path $stage -DestinationPath $zip -CompressionLevel Optimal
Remove-Item -Recurse -Force $stageRoot
Write-Host ("Paket boyutu: {0:N1} MB" -f ((Get-Item $zip).Length/1MB))

# GitHub token'i yerel credential manager'dan al (yazdirilmaz)
# Not: git credential fill stdin'i .NET/PowerShell pipe'indan okuyamiyor; Git'in bash'i uzerinden okunur.
$gitCmd = (Get-Command git).Source
$bash = Join-Path (Split-Path (Split-Path $gitCmd)) "bin\bash.exe"
if(-not (Test-Path $bash)) { $bash = "C:\Program Files\Git\bin\bash.exe" }
if(-not (Test-Path $bash)) { Write-Host "[HATA] Git bash bulunamadi."; exit 1 }
$credOut = & $bash -c "printf 'protocol=https\nhost=github.com\n\n' | git credential fill"
$token = ""
foreach($line in @($credOut)) { $t = "$line".Trim(); if($t -like "password=*") { $token = $t.Substring(9) } }
if(-not $token) { Write-Host "[HATA] GitHub kimligi alinamadi. Bir kez 'git push' yapip giris yapin."; exit 1 }
$headers = @{ Authorization = "Bearer $token"; "User-Agent" = "ElysiumClient-release"; Accept = "application/vnd.github+json" }

# release'i bul; yoksa olustur
$rel = $null
try { $rel = Invoke-RestMethod -Headers $headers -Uri "https://api.github.com/repos/$owner/$repo/releases/tags/$Version" } catch {}
if(-not $rel) {
    Write-Host "Release bulunamadi, olusturuluyor..."
    $body = @{ tag_name = $Version; name = "ElysiumClient $Version"; generate_release_notes = $true } | ConvertTo-Json
    $rel = Invoke-RestMethod -Method Post -Headers $headers -Uri "https://api.github.com/repos/$owner/$repo/releases" -Body $body -ContentType "application/json"
}

# ayni isimli eski asset varsa sil (guncelleme)
foreach($a in @($rel.assets)) {
    if($a.name -eq $zipName) {
        Write-Host "Var olan paket guncelleniyor (eski siliniyor)..."
        Invoke-RestMethod -Method Delete -Headers $headers -Uri $a.url | Out-Null
    }
}

# paketi yukle
Write-Host "GitHub'a yukleniyor (50MB civari, biraz surebilir)..."
$upUrl = "https://uploads.github.com/repos/$owner/$repo/releases/$($rel.id)/assets?name=$zipName"
$asset = Invoke-RestMethod -Method Post -Uri $upUrl -Headers @{ Authorization = "Bearer $token"; "User-Agent" = "ElysiumClient-release" } -ContentType "application/zip" -InFile $zip

Write-Host ""
Write-Host "[OK] Release: $($rel.html_url)"
Write-Host "[OK] Indirme: $($asset.browser_download_url)"

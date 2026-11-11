# Builds Mei's font atlases (fonts/README.md) into <Out>: the pinned Source Han Sans SC
# release from Adobe, checked by SHA-256, its Regular face, the character list of
# tools/mei-charset.py and tools/meifonts.cpp from the build. Takes about six minutes.
#
#   pwsh tools/build-fonts.ps1 -MeiFonts build/Release/meifonts.exe -Out dist/fonts
param(
    [Parameter(Mandatory)][string]$MeiFonts,
    [Parameter(Mandatory)][string]$Out,
    [string]$Work = (Join-Path ([IO.Path]::GetTempPath()) 'mei-fonts')
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent

# Source Han Sans 2.005R, the release upstream/fixtures/open-fonts.json pins too.
$url = 'https://github.com/adobe-fonts/source-han-sans/releases/download/2.005R/09_SourceHanSansSC.zip'
$zipSha = 'ef7364f7ac2564be1ae9c1d74276de2653fe38b73449070398c4fc0b7e032ff1'
$entry = 'OTF/SimplifiedChinese/SourceHanSansSC-Regular.otf'
$fontSha = 'f1d8611151880c6c336aabeac4640ef434fa13cbfbf1ffe82d0a71b2a5637256'

New-Item -ItemType Directory $Work -Force | Out-Null
$zip = Join-Path $Work 'SourceHanSansSC-2.005R.zip'
if (-not (Test-Path $zip) -or (Get-FileHash $zip -Algorithm SHA256).Hash.ToLower() -ne $zipSha) {
    Invoke-WebRequest -Uri $url -OutFile $zip
}
if ((Get-FileHash $zip -Algorithm SHA256).Hash.ToLower() -ne $zipSha) { throw "$zip does not match its SHA-256" }
$font = Join-Path $Work 'SourceHanSansSC-Regular.otf'
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($zip)
try { [IO.Compression.ZipFileExtensions]::ExtractToFile($archive.GetEntry($entry), $font, $true) }
finally { $archive.Dispose() }
if ((Get-FileHash $font -Algorithm SHA256).Hash.ToLower() -ne $fontSha) { throw "$font does not match its SHA-256" }

$chars = Join-Path $Work 'characters.txt'
python -I (Join-Path $root 'tools/mei-charset.py') $chars
if ($LASTEXITCODE) { throw 'mei-charset.py failed' }

# Sizes as the original maps the game's fonts (upstream src/font_assets.cpp); the map
# atlas is smaller than the original's 88 pixels so that all characters fit one page.
New-Item -ItemType Directory $Out -Force | Out-Null
& $MeiFonts $chars $Out --font $font `
    ui-14=14,2048,bc3,white ui-16=16,2048,bc3,white ui-18=18,2048,bc3,white ui-24=24,2048,bc3,white `
    map=48,4096,bc3,black,mips
if ($LASTEXITCODE) { throw 'meifonts failed' }
Copy-Item (Join-Path $root 'fonts/SourceHanSans-OFL.txt') (Join-Path $Out 'OFL.txt')
Get-ChildItem $Out | ForEach-Object { '{0}  {1}' -f (Get-FileHash $_.FullName -Algorithm SHA256).Hash.ToLower(), $_.Name }

# Import-Module -Name Microsoft.PowerShell.Utility
function Get-Version($filePath)
{
    (Get-Item $filePath).VersionInfo.FileVersionRaw | select Major, Minor, Build
}

$version = (Get-Version Release\CsMapfx.exe)
$versionstr = ($version.Major.ToString() + "." + $version.Minor.ToString() + "." + $version.Build.ToString())
& "C:\Program Files (x86)\NSIS\makensisw.exe" "/DMUI_VERSION=$versionstr" "csmapfx.nsi"

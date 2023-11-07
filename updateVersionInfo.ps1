
param([string]$SolutionDir,
     [string]$VersionInfo)

Write-Host "Updating version info of HCM assemblies."
Write-Host "In path: $SolutionDir"
Write-Host "With VersionInfo: $VersionInfo"

$VersionComma = $VersionInfo.Replace('.', ',') # FILEVERSION and PRODUCTVERSION in resource file are comma seperated 

# should match HCMInternal.rc, HCMInterproc.rc, and HCMSpeedhack.rc
Get-Childitem -Path $SolutionDir  -Filter HCM*.rc -Recurse |
Foreach-Object {

	Write-Host "Found file: $_"
	# load content
	$rccontent = Get-Content $_.FullName
	
	# regex replace FILEVERSION and PRODUCTVERSION with new value. CASE SENSITIVE!
	$rccontent = $rccontent -CReplace '\bFILEVERSION\b.*', "FILEVERSION $VersionComma" 
	$rccontent = $rccontent -CReplace '\bPRODUCTVERSION\b.*', "PRODUCTVERSION $VersionComma"
	$rccontent = $rccontent -CReplace 'VALUE "FileVersion".*', "VALUE ""FileVersion"",  ""$VersionInfo"""
	$rccontent = $rccontent -CReplace 'VALUE "ProductVersion".*', "VALUE ""ProductVersion"",  ""$VersionInfo"""

	# save it back
	$rccontent | Set-Content $_.FullName 
	 
	

}
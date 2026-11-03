[Setup]
AppName=LaPautaSL
AppVersion=1.0
DefaultDirName={commonpf}\VST3\LaPautaSL
DefaultGroupName=LaPautaSL
OutputDir=Output
OutputBaseFilename=LaPautaSL-Installer
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "build\LaPautaSL_artefacts\Release\VST3\LaPautaSL.vst3\*"; DestDir: "{commoncf64}\VST3\LaPautaSL.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\LaPautaSL"; Filename: "{app}"

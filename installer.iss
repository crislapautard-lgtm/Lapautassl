; La Pauta SL - Hybrid Edition - Instalador
#define MyAppName "La Pauta SL - SSL Compressor"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "La Pauta Audio Solutions"
#define MyAppURL "https://lapauta.com"

[Setup]
AppId={{A1B2C3D4-1234-5678-9ABC-DEF012345678}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
DefaultDirName={autopf}\La Pauta SL
DisableProgramGroupPage=yes
OutputDir=installer-output
OutputBaseFilename=LaPautaSL-Installer-1.0.0
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "build\LaPautaSL_artefacts\Release\VST3\LaPautaSL.vst3\*"; DestDir: "{commoncf64}\VST3\LaPautaSL.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "build\LaPautaSL_artefacts\Release\VST3\LaPautaSL.vst3\*"; DestDir: "{app}\VST3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autodesktop}\La Pauta SL"; Filename: "{app}"

[Messages]
spanish.BeveledLabel=La Pauta Audio Solutions

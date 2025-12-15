@echo off
chcp 65001
cls
:: [중요] !변수! 문법을 사용하기 위해 반드시 필요한 설정입니다.
setlocal enabledelayedexpansion

set "CurrentPath=%~dp0"

echo [현재 작업 경로]
echo %CurrentPath%
echo.

:: 삭제할 폴더 목록
set LIST=Binaries Intermediate DerivedDataCache

for %%F in (%LIST%) do (
    if exist "%CurrentPath%%%F" (
        echo [삭제 중...] "%CurrentPath%%%F"
        rd /s /q "%CurrentPath%%%F"
        echo  - 삭제 완료.
    ) else (
        echo [없음] "%CurrentPath%%%F" 폴더가 없습니다.
    )
)

echo.
echo [2단계: 프로젝트 파일 생성 준비]

set "UProjectFile="
for %%A in ("%CurrentPath%*.uproject") do (
    set "UProjectFile=%%~fA"
    goto :FoundProject
)

:FoundProject
if not defined UProjectFile (
    echo [오류] .uproject 파일을 찾을 수 없습니다!
    pause
    exit /b
)
echo 타겟 프로젝트: !UProjectFile!

:: [수정됨] 보통 에픽 런처는 Program Files (x86)에 설치됩니다.
set "SelectorPath=C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"

:: 만약 x86에 없다면 그냥 Program Files도 찾아보도록 예외 처리 추가
if not exist "!SelectorPath!" (
    set "SelectorPath=C:\Program Files\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
)

if exist "!SelectorPath!" (
    echo.
    echo -------------------------------------------------------
    echo Generate Visual Studio Project Files 실행 중...
    echo -------------------------------------------------------
    
    "!SelectorPath!" /projectfiles "!UProjectFile!"
    
    echo.
    echo [완료] 프로젝트 파일 생성이 끝났습니다.
) else (
    echo.
    echo [오류] UnrealVersionSelector를 찾을 수 없습니다.
    echo 에픽 런처 설치 경로를 확인해주세요.
)

echo.
pause
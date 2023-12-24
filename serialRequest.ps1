$scriptDir = "[SCRIPT_DIRECTORY]"
$pathToExe = (JOIN-PATH $scriptDir "smartHub.exe")
$toggleLampArgs = "Desk Lamp", "isOn", "auto"
$port = "COM5"
$Baud = "9600"

function read-serial {
    while ($CommPort.IsOpen) {
        $data = $CommPort.ReadLine()
        if ($data -like "ACTION:*") {
            # get the action from format "ACTION: action\n"
            $action = $data.Split(":")[1].Trim()
            if ($action -eq "Toggle lamp") {
                # run the smartHub.exe with the arguements
                Write-Host "TOGGLING LAMP..."
                & $pathToExe $toggleLampArgs
            }
        } else {
            Write-Host $data
        }
    }
}

try {
    Push-Location # Save the current location onto a stack
    Set-Location $scriptDir # Change the directory
    ls
    $CommPort = new-Object System.IO.Ports.SerialPort $port, $Baud, None, 8, one
    $CommPort.Open()
    read-serial
} catch {
    Write-Host $_.Exception.Message
} finally {
    $CommPort.Close()
    Pop-Location # Return to the original location
}


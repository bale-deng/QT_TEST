param(
  [int]$MaxDepth = 3,
  [string[]]$ExcludeDirs = @('build','release','python_runtime','.serena','.claude'),
  [string]$ReadmePath = $(Join-Path (Split-Path -Parent $PSScriptRoot) 'README.md')
)

$repoRoot = Split-Path -Parent $PSScriptRoot

$excludeSet = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::OrdinalIgnoreCase)
foreach ($dir in $ExcludeDirs) {
  [void]$excludeSet.Add($dir)
}

$lines = New-Object 'System.Collections.Generic.List[string]'
$lines.Add('.')

function Add-TreeLines {
  param(
    [string]$Path,
    [string]$Prefix,
    [int]$Depth
  )

  if ($Depth -lt 1) { return }

  $items = Get-ChildItem -LiteralPath $Path -Force | Where-Object {
    if ($_.PSIsContainer) { -not $excludeSet.Contains($_.Name) } else { $true }
  } | Sort-Object @{Expression = { -not $_.PSIsContainer } }, Name

  for ($i = 0; $i -lt $items.Count; $i++) {
    $item = $items[$i]
    $isLast = ($i -eq $items.Count - 1)
    $connector = if ($isLast) { '`--' } else { '|--' }
    $name = if ($item.PSIsContainer) { "$($item.Name)/" } else { $item.Name }
    $lines.Add("$Prefix$connector $name")

    if ($item.PSIsContainer -and $Depth -gt 1) {
      $nextPrefix = $Prefix + $(if ($isLast) { '    ' } else { '|   ' })
      Add-TreeLines -Path $item.FullName -Prefix $nextPrefix -Depth ($Depth - 1)
    }
  }
}

Add-TreeLines -Path $repoRoot -Prefix '' -Depth $MaxDepth

$treeBlock = '```text' + "`n" + ($lines -join "`n") + "`n" + '```'
$start = '<!-- PROJECT_TREE_START -->'
$end = '<!-- PROJECT_TREE_END -->'

if (-not (Test-Path -LiteralPath $ReadmePath)) {
  throw "README not found: $ReadmePath"
}

$content = Get-Content -Raw -LiteralPath $ReadmePath
$pattern = '(?s)' + [regex]::Escape($start) + '.*?' + [regex]::Escape($end)

if ($content -notmatch $pattern) {
  throw 'Project tree markers not found in README.'
}

$replacement = "$start`n$treeBlock`n$end"
$newContent = [regex]::Replace($content, $pattern, $replacement, 1)

if ($newContent -ne $content) {
  Set-Content -LiteralPath $ReadmePath -Value $newContent -Encoding UTF8
  Write-Host "Updated project tree in $ReadmePath"
} else {
  Write-Host 'Project tree already up to date.'
}

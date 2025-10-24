#!/usr/bin/perl
use strict;
use warnings;

# Zgine 项目统计主脚本
# 运行所有统计脚本

print "=== Zgine 项目统计工具 ===\n\n";

my $scripts_dir = "scripts";
my @scripts = (
    {
        name => "文件数量统计",
        file => "count_files.pl",
        description => "统计项目中的文件数量和类型"
    },
    {
        name => "代码行数统计", 
        file => "count_lines.pl",
        description => "统计代码行数、注释行数、空行数"
    },
    {
        name => "项目概览统计",
        file => "project_overview.pl", 
        description => "提供项目的全面统计信息"
    },
    {
        name => "文件类型分析",
        file => "analyze_file_types.pl",
        description => "详细分析不同类型的文件"
    }
);

# 检查脚本文件是否存在
print "检查脚本文件...\n";
my @available_scripts = ();
foreach my $script (@scripts) {
    my $script_path = "$scripts_dir/$script->{file}";
    if (-f $script_path) {
        push @available_scripts, $script;
        print "✓ $script->{name} - $script_path\n";
    } else {
        print "✗ $script->{name} - $script_path (文件不存在)\n";
    }
}

print "\n";

if (@available_scripts == 0) {
    print "没有找到可用的统计脚本!\n";
    exit 1;
}

# 显示菜单
print "可用的统计脚本:\n";
print "-" x 50 . "\n";
for my $i (0..$#available_scripts) {
    printf "%d. %s\n", $i + 1, $available_scripts[$i]{name};
}
print "0. 运行所有脚本\n";
print "q. 退出\n\n";

# 获取用户选择
print "请选择要运行的脚本 (0-" . scalar(@available_scripts) . ", q): ";
my $choice = <STDIN>;
chomp $choice;

if ($choice eq 'q' || $choice eq 'Q') {
    print "退出程序\n";
    exit 0;
}

if ($choice eq '0') {
    # 运行所有脚本
    print "\n运行所有统计脚本...\n";
    print "=" x 60 . "\n";
    
    foreach my $script (@available_scripts) {
        print "\n>>> 运行: $script->{name} <<<\n";
        print "-" x 40 . "\n";
        
        my $script_path = "$scripts_dir/$script->{file}";
        system("perl", $script_path);
        
        print "\n按 Enter 键继续...";
        <STDIN>;
    }
} else {
    # 运行选定的脚本
    my $index = $choice - 1;
    if ($index >= 0 && $index < @available_scripts) {
        my $script = $available_scripts[$index];
        print "\n运行: $script->{name}\n";
        print "描述: $script->{description}\n";
        print "-" x 50 . "\n";
        
        my $script_path = "$scripts_dir/$script->{file}";
        system("perl", $script_path);
    } else {
        print "无效的选择!\n";
        exit 1;
    }
}

print "\n统计完成!\n";

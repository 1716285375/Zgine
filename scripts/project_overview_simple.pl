#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use File::Spec;

# 简化的项目概览统计脚本
# 提供项目的全面统计信息

my $project_root = ".";
my %file_stats = ();
my %dir_stats = ();
my $total_files = 0;
my $total_size = 0;

# 排除的目录
my @exclude_dirs = qw(
    .git
    bin
    bin-int
    vendor
    .cursor
    node_modules
    build
    dist
    obj
    Debug
    Release
    .vs
);

# 主要文件类型
my %main_types = (
    'Source Code' => ['.cpp', '.h', '.hpp', '.c', '.cxx', '.cc'],
    'Scripts' => ['.pl', '.lua', '.bat', '.sh', '.py'],
    'Config' => ['.json', '.ini', '.cfg', '.conf'],
    'Documentation' => ['.md', '.txt', '.rst'],
    'Build Files' => ['.vcxproj', '.sln', '.makefile', '.cmake'],
    'Other' => []
);

print "=== Zgine 项目概览统计 ===\n\n";

# 获取项目信息
my $project_name = "Zgine";
print "项目名称: $project_name\n";
print "统计时间: " . scalar(localtime()) . "\n";
print "项目根目录: " . File::Spec->rel2abs($project_root) . "\n\n";

# 递归查找文件和目录
find(sub {
    my $file = $_;
    my $full_path = $File::Find::name;
    
    # 跳过排除的目录
    foreach my $exclude_dir (@exclude_dirs) {
        return if $full_path =~ /\Q$exclude_dir\E/;
    }
    
    if (-d $file) {
        # 统计目录
        $dir_stats{$file}++;
    } else {
        # 统计文件
        $total_files++;
        
        # 获取文件信息
        my ($name, $path, $ext) = File::Spec->splitpath($file);
        $ext = lc($ext) if $ext;
        
        my $size = -s $full_path;
        $total_size += $size if $size;
        
        # 按扩展名统计
        $file_stats{$ext}++;
        
        # 按类型分类
        my $type = 'Other';
        foreach my $type_name (keys %main_types) {
            foreach my $type_ext (@{$main_types{$type_name}}) {
                if ($ext eq $type_ext) {
                    $type = $type_name;
                    last;
                }
            }
            last if $type ne 'Other';
        }
        $main_types{$type} = [] unless ref $main_types{$type};
        push @{$main_types{$type}}, $ext;
    }
}, $project_root);

# 输出基本统计
print "基本统计:\n";
print "-" x 40 . "\n";
printf "总文件数:     %6d 个\n", $total_files;
printf "总目录数:    %6d 个\n", scalar keys %dir_stats;
printf "总大小:      %6.2f MB\n", $total_size / (1024 * 1024);

print "\n";

# 按文件类型统计
print "按文件类型统计:\n";
print "-" x 50 . "\n";
foreach my $type (sort keys %main_types) {
    my $count = 0;
    my $extensions = {};
    
    foreach my $ext (@{$main_types{$type}}) {
        if (exists $file_stats{$ext}) {
            $count += $file_stats{$ext};
            $extensions->{$ext} = $file_stats{$ext};
        }
    }
    
    if ($count > 0) {
        printf "%-15s %6d 文件\n", $type, $count;
        foreach my $ext (sort keys %$extensions) {
            printf "  %-12s %6d 文件\n", $ext, $extensions->{$ext};
        }
    }
}

print "\n";

# 输出扩展名统计（前10个）
print "文件扩展名统计 (前10个):\n";
print "-" x 30 . "\n";
my @sorted_exts = sort { $file_stats{$b} <=> $file_stats{$a} } keys %file_stats;
my $count = 0;
foreach my $ext (@sorted_exts) {
    last if $count >= 10;
    printf "%-15s %6d 文件\n", $ext || "(无扩展名)", $file_stats{$ext};
    $count++;
}

print "\n";

# 分析项目特点
print "项目特点分析:\n";
print "-" x 40 . "\n";

my $cpp_files = ($file_stats{'.cpp'} || 0) + ($file_stats{'.h'} || 0) + ($file_stats{'.hpp'} || 0);
my $script_files = ($file_stats{'.pl'} || 0) + ($file_stats{'.lua'} || 0) + ($file_stats{'.bat'} || 0);
my $config_files = ($file_stats{'.json'} || 0) + ($file_stats{'.ini'} || 0);
my $doc_files = $file_stats{'.md'} || 0;

printf "C++ 项目文件: %6d 个 (%.1f%%)\n", $cpp_files, ($cpp_files / $total_files) * 100;
printf "脚本文件:     %6d 个 (%.1f%%)\n", $script_files, ($script_files / $total_files) * 100;
printf "配置文件:     %6d 个 (%.1f%%)\n", $config_files, ($config_files / $total_files) * 100;
printf "文档文件:     %6d 个 (%.1f%%)\n", $doc_files, ($doc_files / $total_files) * 100;

# 项目类型判断
if ($cpp_files > $total_files * 0.3) {
    print "\n项目类型: C++ 项目\n";
} elsif ($script_files > $total_files * 0.2) {
    print "\n项目类型: 脚本项目\n";
} else {
    print "\n项目类型: 混合项目\n";
}

print "\n脚本执行完成!\n";

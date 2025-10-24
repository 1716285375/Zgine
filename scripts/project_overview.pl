#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use File::Spec;
use Time::Piece;

# 项目概览统计脚本
# 提供项目的全面统计信息

my $project_root = ".";
my %file_stats = ();
my %dir_stats = ();
my $total_files = 0;
my $total_size = 0;
my $oldest_file = "";
my $newest_file = "";
my $oldest_time = time();
my $newest_time = 0;

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
my $current_time = localtime();
print "项目名称: $project_name\n";
print "统计时间: " . $current_time->strftime("%Y-%m-%d %H:%M:%S") . "\n";
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
        
        my $mtime = (stat($full_path))[9];
        if ($mtime < $oldest_time) {
            $oldest_time = $mtime;
            $oldest_file = $full_path;
        }
        if ($mtime > $newest_time) {
            $newest_time = $mtime;
            $newest_file = $full_path;
        }
        
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

if ($oldest_file) {
    my $oldest_date = localtime($oldest_time)->strftime("%Y-%m-%d %H:%M:%S");
    print "最旧文件:    $oldest_file\n";
    print "修改时间:    $oldest_date\n";
}

if ($newest_file) {
    my $newest_date = localtime($newest_time)->strftime("%Y-%m-%d %H:%M:%S");
    print "最新文件:    $newest_file\n";
    print "修改时间:    $newest_date\n";
}

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

# 按扩展名统计（前10个）
print "文件扩展名统计 (前10个):\n";
print "-" x 30 . "\n";
my @sorted_exts = sort { $file_stats{$b} <=> $file_stats{$a} } keys %file_stats;
my $count = 0;
foreach my $ext (@sorted_exts) {
    last if $count >= 10;
    printf "%-8s %6d 文件\n", $ext || "(无扩展名)", $file_stats{$ext};
    $count++;
}

print "\n";

# 目录结构统计
print "主要目录:\n";
print "-" x 40 . "\n";
my @sorted_dirs = sort keys %dir_stats;
foreach my $dir (@sorted_dirs) {
    next if $dir eq '.';
    printf "%-30s\n", $dir;
}

print "\n";

# 项目健康度评估
print "项目健康度评估:\n";
print "-" x 40 . "\n";

my $cpp_files = 0;
my $h_files = 0;
foreach my $ext ('.cpp', '.c', '.cxx', '.cc') {
    $cpp_files += $file_stats{$ext} if exists $file_stats{$ext};
}
foreach my $ext ('.h', '.hpp') {
    $h_files += $file_stats{$ext} if exists $file_stats{$ext};
}

my $ratio = $h_files > 0 ? $cpp_files / $h_files : 0;
printf "C++ 源文件:   %6d 个\n", $cpp_files;
printf "C++ 头文件:   %6d 个\n", $h_files;
printf "源/头比例:    %6.2f\n", $ratio;

if ($ratio > 0.5 && $ratio < 2.0) {
    print "代码结构:     良好\n";
} elsif ($ratio > 2.0) {
    print "代码结构:     头文件偏少\n";
} else {
    print "代码结构:     头文件偏多\n";
}

print "\n脚本执行完成!\n";

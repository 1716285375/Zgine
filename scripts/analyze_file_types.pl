#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use File::Spec;

# 按文件类型分类统计脚本
# 详细分析不同类型的文件

my $project_root = ".";
my %type_stats = ();
my %ext_stats = ();

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

# 文件类型定义
my %file_types = (
    'C++ Source' => {
        extensions => ['.cpp', '.cxx', '.cc', '.c'],
        description => 'C++ 源文件',
        icon => '🔧'
    },
    'C++ Headers' => {
        extensions => ['.h', '.hpp'],
        description => 'C++ 头文件',
        icon => '📋'
    },
    'Scripts' => {
        extensions => ['.pl', '.py', '.lua', '.bat', '.sh'],
        description => '脚本文件',
        icon => '📜'
    },
    'Configuration' => {
        extensions => ['.json', '.ini', '.cfg', '.conf', '.yml', '.yaml'],
        description => '配置文件',
        icon => '⚙️'
    },
    'Documentation' => {
        extensions => ['.md', '.txt', '.rst', '.doc', '.docx'],
        description => '文档文件',
        icon => '📚'
    },
    'Build Files' => {
        extensions => ['.vcxproj', '.sln', '.makefile', '.cmake', '.lua'],
        description => '构建文件',
        icon => '🔨'
    },
    'Graphics' => {
        extensions => ['.png', '.jpg', '.jpeg', '.gif', '.bmp', '.svg'],
        description => '图像文件',
        icon => '🖼️'
    },
    'Audio/Video' => {
        extensions => ['.mp3', '.wav', '.mp4', '.avi', '.mov'],
        description => '音视频文件',
        icon => '🎵'
    },
    'Archives' => {
        extensions => ['.zip', '.rar', '.tar', '.gz', '.7z'],
        description => '压缩文件',
        icon => '📦'
    },
    'Other' => {
        extensions => [],
        description => '其他文件',
        icon => '📄'
    }
);

print "=== Zgine 项目文件类型详细统计 ===\n\n";

# 递归查找文件
find(sub {
    my $file = $_;
    my $full_path = $File::Find::name;
    
    # 跳过目录
    return if -d $file;
    
    # 跳过排除的目录
    foreach my $exclude_dir (@exclude_dirs) {
        return if $full_path =~ /\Q$exclude_dir\E/;
    }
    
    # 获取文件扩展名
    my ($name, $path, $ext) = File::Spec->splitpath($file);
    $ext = lc($ext) if $ext;
    
    # 统计扩展名
    $ext_stats{$ext}++;
    
    # 分类文件类型
    my $classified = 0;
    foreach my $type (keys %file_types) {
        foreach my $type_ext (@{$file_types{$type}{extensions}}) {
            if ($ext eq $type_ext) {
                $type_stats{$type}{count}++;
                $type_stats{$type}{extensions}{$ext}++;
                $classified = 1;
                last;
            }
        }
        last if $classified;
    }
    
    # 如果未分类，归为 Other
    unless ($classified) {
        $type_stats{'Other'}{count}++;
        $type_stats{'Other'}{extensions}{$ext}++;
    }
    
}, $project_root);

# 输出按类型统计
print "按文件类型统计:\n";
print "=" x 60 . "\n";

my $total_files = 0;
foreach my $type (sort keys %type_stats) {
    my $count = $type_stats{$type}{count};
    $total_files += $count;
    
    if ($count > 0) {
        my $icon = $file_types{$type}{icon};
        my $desc = $file_types{$type}{description};
        printf "%s %-20s %6d 文件\n", $icon, $desc, $count;
        
        # 显示该类型下的扩展名
        foreach my $ext (sort keys %{$type_stats{$type}{extensions}}) {
            my $ext_count = $type_stats{$type}{extensions}{$ext};
            printf "    %-10s %6d 文件\n", $ext, $ext_count;
        }
        print "\n";
    }
}

print "=" x 60 . "\n";
printf "总计: %6d 文件\n\n", $total_files;

# 输出扩展名统计
print "所有文件扩展名统计:\n";
print "-" x 40 . "\n";
my @sorted_exts = sort { $ext_stats{$b} <=> $ext_stats{$a} } keys %ext_stats;
foreach my $ext (@sorted_exts) {
    printf "%-10s %6d 文件\n", $ext || "(无扩展名)", $ext_stats{$ext};
}

print "\n";

# 分析项目特点
print "项目特点分析:\n";
print "-" x 40 . "\n";

my $cpp_files = ($type_stats{'C++ Source'}{count} || 0) + ($type_stats{'C++ Headers'}{count} || 0);
my $script_files = $type_stats{'Scripts'}{count} || 0;
my $config_files = $type_stats{'Configuration'}{count} || 0;
my $doc_files = $type_stats{'Documentation'}{count} || 0;

printf "C++ 项目文件: %6d 个 (%.1f%%)\n", $cpp_files, ($cpp_files / $total_files) * 100;
printf "脚本文件:     %6d 个 (%.1f%%)\n", $script_files, ($script_files / $total_files) * 100;
printf "配置文件:     %6d 个 (%.1f%%)\n", $config_files, ($config_files / $total_files) * 100;
printf "文档文件:     %6d 个 (%.1f%%)\n", $doc_files, ($doc_files / $total_files) * 100;

# 项目类型判断
if ($cpp_files > $total_files * 0.5) {
    print "\n项目类型: C++ 项目\n";
} elsif ($script_files > $total_files * 0.3) {
    print "\n项目类型: 脚本项目\n";
} else {
    print "\n项目类型: 混合项目\n";
}

print "\n脚本执行完成!\n";

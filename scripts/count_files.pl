#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use File::Spec;

# 文件统计脚本
# 统计项目中的文件数量和类型

my $project_root = ".";
my %file_stats = ();
my $total_files = 0;

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

# 排除的文件扩展名
my @exclude_extensions = qw(
    .exe
    .dll
    .lib
    .obj
    .pdb
    .idb
    .tlog
    .log
    .ini
    .user
    .filters
);

print "=== Zgine 项目文件统计 ===\n\n";

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
    
    # 跳过排除的扩展名
    foreach my $exclude_ext (@exclude_extensions) {
        return if $ext eq $exclude_ext;
    }
    
    # 统计文件
    $total_files++;
    $file_stats{$ext}++;
    
}, $project_root);

# 输出统计结果
print "总文件数: $total_files\n\n";

print "按文件类型分类 (前20个):\n";
print "-" x 40 . "\n";

# 按扩展名排序，只显示前20个
my @sorted_exts = sort { $file_stats{$b} <=> $file_stats{$a} } keys %file_stats;
my $count = 0;
foreach my $ext (@sorted_exts) {
    last if $count >= 20;
    my $file_count = $file_stats{$ext};
    my $percentage = sprintf("%.1f", ($file_count / $total_files) * 100);
    printf "%-15s %6d 文件 (%5.1f%%)\n", $ext || "(无扩展名)", $file_count, $percentage;
    $count++;
}

print "\n";

# 统计主要代码文件
my %code_extensions = (
    '.cpp' => 'C++ 源文件',
    '.h'   => 'C++ 头文件',
    '.hpp' => 'C++ 头文件',
    '.c'   => 'C 源文件',
    '.pl'  => 'Perl 脚本',
    '.lua' => 'Lua 脚本',
    '.md'  => 'Markdown 文档',
    '.json' => 'JSON 配置文件',
    '.bat' => '批处理文件',
    '.sh'  => 'Shell 脚本'
);

print "主要代码文件统计:\n";
print "-" x 40 . "\n";
my $code_files = 0;
foreach my $ext (sort keys %code_extensions) {
    if (exists $file_stats{$ext}) {
        my $count = $file_stats{$ext};
        $code_files += $count;
        printf "%-20s %6d 文件\n", $code_extensions{$ext}, $count;
    }
}

print "-" x 40 . "\n";
printf "%-20s %6d 文件\n", "代码文件总计", $code_files;

print "\n脚本执行完成!\n";

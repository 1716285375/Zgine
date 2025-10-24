#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use File::Spec;

# 代码行数统计脚本
# 统计项目中的代码行数、注释行数、空行数

my $project_root = ".";
my %stats = ();
my $total_lines = 0;
my $total_code_lines = 0;
my $total_comment_lines = 0;
my $total_empty_lines = 0;

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

# 代码文件扩展名
my %code_extensions = (
    '.cpp' => 'C++',
    '.h'   => 'C++',
    '.hpp' => 'C++',
    '.c'   => 'C',
    '.pl'  => 'Perl',
    '.lua' => 'Lua',
    '.js'  => 'JavaScript',
    '.ts'  => 'TypeScript',
    '.py'  => 'Python',
    '.java' => 'Java',
    '.cs'  => 'C#',
    '.php' => 'PHP',
    '.rb'  => 'Ruby',
    '.go'  => 'Go',
    '.rs'  => 'Rust',
    '.sh'  => 'Shell',
    '.bat' => 'Batch'
);

print "=== Zgine 项目代码行数统计 ===\n\n";

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
    
    # 只处理代码文件
    return unless exists $code_extensions{$ext};
    
    # 读取文件内容
    open my $fh, '<', $full_path or return;
    my $lines = 0;
    my $code_lines = 0;
    my $comment_lines = 0;
    my $empty_lines = 0;
    
    while (my $line = <$fh>) {
        $lines++;
        $line =~ s/\r?\n$//;  # 移除换行符
        
        # 检查空行
        if ($line =~ /^\s*$/) {
            $empty_lines++;
            next;
        }
        
        # 检查注释行（根据文件类型）
        my $is_comment = 0;
        if ($ext eq '.cpp' || $ext eq '.h' || $ext eq '.hpp' || $ext eq '.c') {
            # C/C++ 注释
            $is_comment = 1 if $line =~ /^\s*\/\// || $line =~ /^\s*\/\*/ || $line =~ /\*\//;
        } elsif ($ext eq '.pl') {
            # Perl 注释
            $is_comment = 1 if $line =~ /^\s*#/;
        } elsif ($ext eq '.lua') {
            # Lua 注释
            $is_comment = 1 if $line =~ /^\s*--/;
        } elsif ($ext eq '.js' || $ext eq '.ts') {
            # JavaScript/TypeScript 注释
            $is_comment = 1 if $line =~ /^\s*\/\// || $line =~ /^\s*\/\*/ || $line =~ /\*\//;
        } elsif ($ext eq '.py') {
            # Python 注释
            $is_comment = 1 if $line =~ /^\s*#/;
        } elsif ($ext eq '.java' || $ext eq '.cs') {
            # Java/C# 注释
            $is_comment = 1 if $line =~ /^\s*\/\// || $line =~ /^\s*\/\*/ || $line =~ /\*\//;
        } elsif ($ext eq '.php') {
            # PHP 注释
            $is_comment = 1 if $line =~ /^\s*\/\// || $line =~ /^\s*#/ || $line =~ /^\s*\/\*/ || $line =~ /\*\//;
        } elsif ($ext eq '.rb') {
            # Ruby 注释
            $is_comment = 1 if $line =~ /^\s*#/;
        } elsif ($ext eq '.go') {
            # Go 注释
            $is_comment = 1 if $line =~ /^\s*\/\// || $line =~ /^\s*\/\*/ || $line =~ /\*\//;
        } elsif ($ext eq '.rs') {
            # Rust 注释
            $is_comment = 1 if $line =~ /^\s*\/\// || $line =~ /^\s*\/\*/ || $line =~ /\*\//;
        } elsif ($ext eq '.sh') {
            # Shell 注释
            $is_comment = 1 if $line =~ /^\s*#/;
        }
        
        if ($is_comment) {
            $comment_lines++;
        } else {
            $code_lines++;
        }
    }
    
    close $fh;
    
    # 更新统计
    $total_lines += $lines;
    $total_code_lines += $code_lines;
    $total_comment_lines += $comment_lines;
    $total_empty_lines += $empty_lines;
    
    # 按语言分类统计
    my $language = $code_extensions{$ext};
    $stats{$language}{files}++;
    $stats{$language}{lines} += $lines;
    $stats{$language}{code_lines} += $code_lines;
    $stats{$language}{comment_lines} += $comment_lines;
    $stats{$language}{empty_lines} += $empty_lines;
    
}, $project_root);

# 输出总体统计
print "总体统计:\n";
print "-" x 50 . "\n";
printf "总文件数:     %6d 个\n", scalar keys %stats;
printf "总行数:       %6d 行\n", $total_lines;
if ($total_lines > 0) {
    printf "代码行数:     %6d 行 (%.1f%%)\n", $total_code_lines, ($total_code_lines / $total_lines) * 100;
    printf "注释行数:     %6d 行 (%.1f%%)\n", $total_comment_lines, ($total_comment_lines / $total_lines) * 100;
    printf "空行数:       %6d 行 (%.1f%%)\n", $total_empty_lines, ($total_empty_lines / $total_lines) * 100;
} else {
    printf "代码行数:     %6d 行\n", $total_code_lines;
    printf "注释行数:     %6d 行\n", $total_comment_lines;
    printf "空行数:       %6d 行\n", $total_empty_lines;
}

print "\n按编程语言分类:\n";
print "-" x 80 . "\n";
printf "%-12s %6s %8s %8s %8s %8s\n", "语言", "文件", "总行数", "代码行", "注释行", "空行";
print "-" x 80 . "\n";

foreach my $language (sort keys %stats) {
    my $data = $stats{$language};
    printf "%-12s %6d %8d %8d %8d %8d\n", 
        $language, 
        $data->{files}, 
        $data->{lines}, 
        $data->{code_lines}, 
        $data->{comment_lines}, 
        $data->{empty_lines};
}

print "\n";

# 计算平均每个文件的代码行数
if ($total_lines > 0 && scalar keys %stats > 0) {
    my $avg_lines_per_file = $total_lines / scalar keys %stats;
    printf "平均每个文件: %.1f 行\n", $avg_lines_per_file;
}

print "\n脚本执行完成!\n";

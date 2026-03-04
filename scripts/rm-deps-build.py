#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse
import os
import shutil
from pathlib import Path


def remove_build_folders(target_dir, confirm=True, verbose=True):
    """
    移除指定目录下所有以 -subbuild 或 -build 结尾的文件夹

    Args:
        target_dir (str): 目标目录路径
        confirm (bool): 是否需要用户确认
        verbose (bool): 是否打印详细信息
    """
    dir_path = Path(target_dir)

    # 检查目录是否存在
    if not dir_path.exists():
        print(f"错误: 目录 '{target_dir}' 不存在。")
        return False
    if not dir_path.is_dir():
        print(f"错误: '{target_dir}' 不是一个目录。")
        return False

    # 定义匹配后缀
    suffixes = ("-subbuild", "-build")

    # 查找所有匹配的文件夹
    folders_to_remove = []
    for item in dir_path.iterdir():
        if item.is_dir() and (
            item.name.endswith("-subbuild") or item.name.endswith("-build")
        ):
            folders_to_remove.append(item)

    # 如果没有找到
    if not folders_to_remove:
        if verbose:
            print(f"在 '{target_dir}' 中未发现以 -subbuild 或 -build 结尾的文件夹。")
        return True

    # 显示列表
    print(f"在 '{target_dir}' 中找到以下文件夹将被删除:")
    for folder in folders_to_remove:
        print(f"  🗑️  {folder}")

    # 确认步骤
    if confirm:
        response = input("\n确认删除上述所有文件夹？[y/N]: ")
        if response.lower() not in ["y", "yes"]:
            print("操作已取消。")
            return False

    # 执行删除
    print("\n正在删除...")
    success_count = 0
    fail_count = 0

    for folder in folders_to_remove:
        try:
            shutil.rmtree(folder)
            if verbose:
                print(f"✅ 成功删除: {folder}")
            success_count += 1
        except Exception as e:
            print(f"❌ 删除失败 {folder}: {e}")
            fail_count += 1

    print(f"\n--- 完成 ---")
    print(f"成功: {success_count}, 失败: {fail_count}")
    return fail_count == 0


def main():
    parser = argparse.ArgumentParser(
        description="移除指定目录下所有以 -subbuild 或 -build 结尾的文件夹",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s /path/to/your/project      # 交互式确认删除
  %(prog)s /path/to/your/project -y   # 强制删除，不提示确认
  %(prog)s .                          # 删除当前目录下的文件夹
        """,
    )

    parser.add_argument(
        "directory", type=str, help="目标目录的路径 (例如: . 或 /home/user/project)"
    )

    parser.add_argument(
        "-y", "--yes", action="store_true", help="跳过确认提示，直接删除 (强制模式)"
    )

    parser.add_argument(
        "-q", "--quiet", action="store_true", help="静默模式，减少输出信息"
    )

    args = parser.parse_args()

    # 执行删除操作
    remove_build_folders(
        target_dir=args.directory,
        confirm=not args.yes,  # 如果传入了 -y，则不需要确认
        verbose=not args.quiet,  # 如果传入了 -q，则不打印详细信息
    )


if __name__ == "__main__":
    main()

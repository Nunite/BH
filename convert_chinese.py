#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import os
import sys
import zhconv
import argparse


def convert_quotes_to_traditional(content):
    """
    将双引号内的简体中文转换为繁体中文
    """

    def convert_match(match):
        # 获取匹配的字符串（包括双引号）
        quoted_text = match.group(0)
        # 去掉双引号
        text = quoted_text[1:-1]
        # 检查是否包含中文字符
        if any("\u4e00" <= ch <= "\u9fff" for ch in text):
            # 转换为繁体中文（台湾标准）
            traditional_text = zhconv.convert(text, "zh-tw")
            return f'"{traditional_text}"'
        return quoted_text

    # 正则表达式匹配双引号内的文本，但排除转义的双引号 \"
    pattern = r'(?<!\\)"(.*?(?<!\\))"'
    return re.sub(pattern, convert_match, content, flags=re.DOTALL)


def process_file(file_path, output_path=None, backup=True):
    """
    处理文件，将双引号中的简体中文转为繁体中文

    参数:
        file_path: 输入文件路径
        output_path: 输出文件路径，如果为None则覆盖原文件
        backup: 是否创建备份文件
    """
    try:
        # 读取文件内容
        with open(file_path, "r", encoding="utf-8") as file:
            content = file.read()

        # 如果需要创建备份
        if backup:
            backup_path = file_path + ".bak"
            with open(backup_path, "w", encoding="utf-8") as backup_file:
                backup_file.write(content)
            print(f"已创建备份文件: {backup_path}")

        # 转换内容
        converted_content = convert_quotes_to_traditional(content)

        # 写入输出文件
        if output_path:
            with open(output_path, "w", encoding="utf-8") as out_file:
                out_file.write(converted_content)
            print(f"转换结果已写入: {output_path}")
        else:
            with open(file_path, "w", encoding="utf-8") as file:
                file.write(converted_content)
            print(f"已更新文件: {file_path}")

        return True
    except Exception as e:
        print(f"处理文件 {file_path} 时出错: {str(e)}")
        return False


def main():
    parser = argparse.ArgumentParser(
        description="将代码中双引号内的简体中文转换为繁体中文"
    )
    parser.add_argument("input", help="输入文件或目录路径")
    parser.add_argument(
        "-o", "--output", help="输出文件路径（仅当输入为单个文件时有效）"
    )
    parser.add_argument("--no-backup", action="store_true", help="不创建备份文件")
    parser.add_argument(
        "-r", "--recursive", action="store_true", help="递归处理目录中的所有文件"
    )
    parser.add_argument(
        "-e",
        "--extensions",
        default=".cpp,.h,.c,.hpp",
        help="要处理的文件扩展名，用逗号分隔 (默认: .cpp,.h,.c,.hpp)",
    )

    args = parser.parse_args()

    input_path = args.input
    create_backup = not args.no_backup
    extensions = args.extensions.split(",")

    if os.path.isfile(input_path):
        # 处理单个文件
        process_file(input_path, args.output, create_backup)
    elif os.path.isdir(input_path):
        # 处理目录
        if args.output:
            print("警告: 处理目录时忽略输出路径参数")

        processed_files = 0
        for root, dirs, files in os.walk(input_path):
            for file in files:
                if any(file.endswith(ext) for ext in extensions):
                    file_path = os.path.join(root, file)
                    if process_file(file_path, None, create_backup):
                        processed_files += 1

            if not args.recursive:
                break  # 如果不递归，只处理顶层目录

        print(f"共处理了 {processed_files} 个文件")
    else:
        print(f"错误: 输入路径 '{input_path}' 不存在")
        sys.exit(1)


if __name__ == "__main__":
    main()

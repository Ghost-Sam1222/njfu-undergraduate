# NJFU Undergraduate — Claude Code 工作协议

## 分工模型

```
Claude Code（整理）→ 用户/Codex（审计提交）→ Claude Code（清理确认）
```

Claude Code **不负责** git commit/push。最后一步交给用户或 Codex 完成。

## 工作流

### 阶段 1：整理课程资料

1. 接收用户指定的课程名称
2. 探索本地源目录，了解文件结构
3. 在仓库中创建课程目录，按统一结构组织：
   ```
   课程名/
   ├── README.md       # 课号、学分、学期、资料清单、备注
   ├── 课件/
   ├── 往年真题/
   ├── 复习资料/
   └── ...（按需）
   ```
4. 跳过：教材 PDF（版权+体积）、.DS_Store、临时文件
5. 脱敏：文件名中的学号/姓名须去除
6. AI 生成内容须在 README 中标注
7. 更新顶层 README.md 导航表

### 阶段 2：交接摘要

每轮整理完成后，输出固定格式：

```
本轮新增/修改课程：XXX
新增文件数量：N
删除/移动文件：N
需要 Codex 特别检查的路径：
  - path/to/check
```

### 阶段 3：用户/Codex 审计提交

用户或 Codex 执行：
```bash
git status --short
git diff --stat
git add -A
git commit -m "<描述>"
git push
```

### 阶段 4：云端确认 + 删除本地源文件

**4a. 启动子 agent 做云-本比对：**
使用 Explore 子 agent，对比 GitHub 仓库文件与本地源目录，逐文件确认入库状态。

**4b. 输出可删除清单：**
| 本地路径 | 大小 | 云端状态 | 建议 |
|----------|------|----------|------|
| ... | ... | ✅/❌/⚠️ | 可删/保留/检查 |

**4c. 用户确认后，执行删除。**

## 行为准则

- 不主动删除用户本地文件，必须经用户确认
- 教材 PDF 一律不入库（版权+体积）
- 他人学号/姓名必须脱敏
- 复习资料如为 AI 辅助生成，必须在 README 中标注
- 每门课一个 README.md 做入口，写清楚课号、学分、资料清单

## 环境

Claude Code shell 不自动继承 Homebrew PATH。如需使用 `gh`：
```bash
eval "$(/opt/homebrew/bin/brew shellenv)"
```

#!/bin/bash
set -e

DESIGN_DIR="/home/sz/coderepo/study/data_Structure_work/design"
DIST_DIR="${DESIGN_DIR}/dist/QtApps"
QT_PLUGINS="/usr/lib/x86_64-linux-gnu/qt5/plugins"

rm -rf "${DIST_DIR}"
mkdir -p "${DIST_DIR}"

copy_deps() {
    local bin="$1"
    local dest="$2"
    local lib

    while IFS= read -r line; do
        lib=$(echo "$line" | awk 'NF>=3 && $2=="=>"{print $3}')
        [ -z "$lib" ] && continue
        [ ! -f "$lib" ] && continue
        local base=$(basename "$lib")
        [ -f "${dest}/${base}" ] && continue
        cp -L "$lib" "${dest}/${base}"
        chmod +w "${dest}/${base}"
    done < <(ldd "$bin")
}

echo "=== 创建发布目录结构 ==="
mkdir -p "${DIST_DIR}/bin"
mkdir -p "${DIST_DIR}/lib"
mkdir -p "${DIST_DIR}/plugins/platforms"
mkdir -p "${DIST_DIR}/plugins/imageformats"
mkdir -p "${DIST_DIR}/plugins/platformthemes"
mkdir -p "${DIST_DIR}/data"

echo "=== 复制可执行文件 ==="
cp "${DESIGN_DIR}/student_query_system/student_query_system" "${DIST_DIR}/bin/"
cp "${DESIGN_DIR}/DeepSeekConfigTool/DeepSeekConfigTool" "${DIST_DIR}/bin/"
chmod +x "${DIST_DIR}/bin/student_query_system" "${DIST_DIR}/bin/DeepSeekConfigTool"

echo "=== 设置 RPATH ==="
patchelf --set-rpath '$ORIGIN/../lib' "${DIST_DIR}/bin/student_query_system"
patchelf --set-rpath '$ORIGIN/../lib' "${DIST_DIR}/bin/DeepSeekConfigTool"

echo "=== 收集共享库依赖 ==="
copy_deps "${DIST_DIR}/bin/student_query_system" "${DIST_DIR}/lib"
copy_deps "${DIST_DIR}/bin/DeepSeekConfigTool" "${DIST_DIR}/lib"

echo "=== 复制 Qt 插件 ==="
cp -L "${QT_PLUGINS}/platforms/libqxcb.so" "${DIST_DIR}/plugins/platforms/"
cp -L "${QT_PLUGINS}/platforms/libqoffscreen.so" "${DIST_DIR}/plugins/platforms/"
cp -L "${QT_PLUGINS}/platforms/libqminimal.so" "${DIST_DIR}/plugins/platforms/"
cp -L "${QT_PLUGINS}/imageformats/libqjpeg.so" "${DIST_DIR}/plugins/imageformats/"
cp -L "${QT_PLUGINS}/imageformats/libqgif.so" "${DIST_DIR}/plugins/imageformats/"
cp -L "${QT_PLUGINS}/imageformats/libqsvg.so" "${DIST_DIR}/plugins/imageformats/"
if [ -f "${QT_PLUGINS}/platformthemes/libqgtk3.so" ]; then
    cp -L "${QT_PLUGINS}/platformthemes/libqgtk3.so" "${DIST_DIR}/plugins/platformthemes/"
fi

echo "=== 收集插件依赖 ==="
for so in "${DIST_DIR}/plugins"/**/*.so; do
    copy_deps "$so" "${DIST_DIR}/lib"
done

echo "=== 复制数据文件 ==="
cp "${DESIGN_DIR}/资料/tab_student.csv" "${DIST_DIR}/data/"
cp "${DESIGN_DIR}/资料/tab_course.csv" "${DIST_DIR}/data/"
cp "${DESIGN_DIR}/资料/tab_class.csv" "${DIST_DIR}/data/"
cp "${DESIGN_DIR}/资料/tab_score.csv" "${DIST_DIR}/data/"

echo "=== 创建启动脚本 ==="
cat > "${DIST_DIR}/student_query_system.sh" << 'LAUNCHER'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${SCRIPT_DIR}/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="${SCRIPT_DIR}/plugins/platforms"
exec "${SCRIPT_DIR}/bin/student_query_system" "$@"
LAUNCHER
chmod +x "${DIST_DIR}/student_query_system.sh"

cat > "${DIST_DIR}/DeepSeekConfigTool.sh" << 'LAUNCHER'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${SCRIPT_DIR}/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="${SCRIPT_DIR}/plugins/platforms"
exec "${SCRIPT_DIR}/bin/DeepSeekConfigTool" "$@"
LAUNCHER
chmod +x "${DIST_DIR}/DeepSeekConfigTool.sh"

echo "=== 打包 ==="
cd "${DESIGN_DIR}/dist"
tar czf QtApps.tar.gz QtApps/

echo ""
echo "=== 完成 ==="
echo "发布包: ${DESIGN_DIR}/dist/QtApps.tar.gz"
echo "大小: $(du -sh ${DESIGN_DIR}/dist/QtApps.tar.gz | cut -f1)"
echo ""
echo "使用方法："
echo "  1. 解压: tar xzf QtApps.tar.gz"
echo "  2. 运行学生查询系统: ./QtApps/student_query_system.sh"
echo "  3. 运行DeepSeek工具: ./QtApps/DeepSeekConfigTool.sh"
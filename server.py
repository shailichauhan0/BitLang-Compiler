from flask import Flask, request, jsonify, send_file
import subprocess
import os
import time

app = Flask(__name__)

# === Paths ===
BUILD_DIR = "build"
TEST_PROG = os.path.join(BUILD_DIR, "test.prog")
PROJECT_DIR = os.path.abspath(".")  # /mnt/.../OurMiniCompiler9

@app.route("/")
def serve_html():
    return send_file("a.html")

@app.route("/compile", methods=["POST"])
def compile_code():
    try:
        print("🟡 /compile endpoint was hit")
        data = request.get_json()
        code = data.get("code", "")

        with open(TEST_PROG, "w") as f:
            f.write(code)

        start = time.time()

        # === 1. Run cmake .. ===
        cmake_config = subprocess.run(
            ["cmake", os.path.abspath(".")],
            cwd=BUILD_DIR,
            capture_output=True,
            text=True
        )
        cmake_out = cmake_config.stdout + cmake_config.stderr
        print("✅ cmake done")

        # === 2. Run cmake --build . ===
        build = subprocess.run(
            ["cmake", "--build", "."],
            cwd=BUILD_DIR,
            capture_output=True,
            text=True
        )
        build_out = build.stdout + build.stderr
        print("✅ build done")

        full_command_output = f"$ cmake ..\n{cmake_out}\n\n$ cmake --build .\n{build_out}"

        # === 3. Extract AST & Semantic ===
        ast_semantic_part = ""
        try:
            lines = build_out.splitlines()
            collected_parts = []

            # 1. Get "Parsed successfully" + 1 lines after
            try:
                parsed_idx = next(i for i, line in enumerate(lines) if "Parsed successfully" in line.strip())
                collected_parts.extend(lines[parsed_idx : parsed_idx + 2])
            except StopIteration:
                pass

            # 2. Get "100%] Built target run" + all lines after it
            try:
                run_idx = next(i for i, line in enumerate(lines) if "[100%] Built target run" in line.strip())
                collected_parts.extend(lines[run_idx:])
            except StopIteration:
                pass

            # 3. Get from "Semantic analysis completed successfully" to "Generating LLVM IR"
            try:
                sem_ok_idx = next(i for i, line in enumerate(lines) if "Semantic analysis completed successfully" in line.strip())
                gen_llvm_idx = next(i for i, line in enumerate(lines[sem_ok_idx:], start=sem_ok_idx)
                                    if "Generating LLVM IR" in line.strip())
                collected_parts.extend(lines[sem_ok_idx : gen_llvm_idx + 1])
            except StopIteration:
                pass

            # Final assembly
            ast_semantic_part = "\n".join(collected_parts)
        except Exception as e:
            ast_semantic_part = "❌ AST & Semantic block not found."

        # === 4. Read output.ll (LLVM IR) ===
        try:
            with open(os.path.join(BUILD_DIR, "output.ll")) as f:
                llvm_ir = f.read()
        except:
            llvm_ir = "❌ LLVM IR not generated."

        # === 5. Extract final output from build log ===
        try:
            idx = build_out.index("Generating LLVM IR")
            output_part = build_out[idx:]
            output_lines = output_part.splitlines()
            final_output_lines = []
            found_ir = False
            for line in output_lines:
                if "LLVM IR written to output.ll" in line:
                    found_ir = True
                    continue
                if found_ir:
                    final_output_lines.append(line)
            execution_result = "\n".join(final_output_lines).strip()
        except:
            execution_result = "❌ Could not extract final output."

        end = time.time()

        print("Returning:\n"
            f"🔹 command_log:\n{full_command_output}\n\n"
            f"🔹 ast_semantic:\n{ast_semantic_part.strip()}\n\n"
            f"🔹 llvm_ir:\n{llvm_ir}\n\n"
            f"🔹 execution_result:\n{execution_result.strip()}\n\n"
            f"🔹 compile_time: {round((end - start) * 1000)}ms\n")

        # === Response ===
        return jsonify({
            "command_log": full_command_output,
            "ast_semantic": ast_semantic_part.strip(),
            "llvm_ir": llvm_ir,
            "execution_result": execution_result,
            "compile_time": f"{round((end - start) * 1000)}ms"
        })

    except Exception as e:
        print("❌ Top-level error:", str(e))
        return jsonify({"error": str(e)}), 500

# === Run Server ===
if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0",port=5001)

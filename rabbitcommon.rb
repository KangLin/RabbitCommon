# https://docs.brew.sh/Formula-Cookbook

class rabbitcommon < Formula
  desc " Rabbit common library"
  homepage "https://github.com/KangLin/RabbitCommon"
  license "GNU General Public License v3.0"
  # 明确使用 develop 分支
  url "https://github.com/KangLin/RabbitCommon.git",
      tag: "v2.3.4",
      revision: "efac9b39cd804263f7adff180b9b42db9ba99bc5"

  # 使用 `brew install --HEAD RabbitCommon` 来得到 develop 分支
  head "https://github.com/KangLin/RabbitCommon.git", branch: "develop"

  depends_on "cmake" => [:build, :test]
  depends_on "ninja" => :build
  depends_on "pkg-config" => :build
  depends_on "graphviz" => :build
  depends_on "gettext" => :build
  depends_on "doxygen" => :build

  depends_on "openssl"
  depends_on "curl"
  depends_on "qtbase"
  depends_on "qtscxml"
  depends_on "qtmultimedia"
  depends_on "qtwebengine"
  depends_on "qtserialport"

  # 如果是GUI应用
  depends_on "qwt" if build.with? "qwt"

  def install
    args = std_cmake_args # + %W[
    #  -DCMAKE_INSTALL_PREFIX=#{prefix}
    #  -DCMAKE_BUILD_TYPE=Release
    #]
    
    # 创建build目录
    mkdir "build" do
      system "cmake", "..", *args
      system "make", "-j#{ENV.make_jobs}"
      system "make", "install"
    end
    
    # 安装应用程序（如果是macOS .app）
    if OS.mac? && build.stable?
      prefix.install Dir["build/*.app"]
      bin.write_exec_script prefix/"RabbitCommon.app/Contents/MacOS/RabbitCommon"
    end
  end

  def caveats
    <<~EOS
      RabbitCommon has been installed!
      
      To run the application:
        RabbitCommon
      
      For GUI version on macOS, you can find it in:
        #{opt_prefix}/RabbitCommon.app
    EOS
  end

  test do
    # 测试命令行版本
    system "#{bin}/RabbitCommon", "--help"
    
    # 测试版本信息
    assert_match version.to_s, shell_output("#{bin}/RabbitCommon --version")
  end
end

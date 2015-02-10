task :default => "build"

desc "Build Application"
task :build do |t|
  Dir.chdir("scripts") do
    sh "root -b -q -l loadClasses.C"
  end
end

desc "2D Event Display"
task "2d" do |t|
  Dir.chdir("scripts") do
    sh "root -l run2d.C"
  end
end

desc "3D Event Display"
task "3d" do |t|
  Dir.chdir("scripts") do
    sh "root -l run3d.C"
  end
end

desc "Show Full 3D Geometry (OpenGL)"
task "geo" do |t|
  Dir.chdir("geometry") do
    sh "root -l gl.C"
  end
end

desc "Test"
task "test" do |t|
  Dir.chdir("scripts") do
    # provide your own test case
    sh "root -b -q -l test.C"
  end
end
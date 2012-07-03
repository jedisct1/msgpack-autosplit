require 'msgpack'

Given /^a file named "(.*?)" with short msgpack data$/ do |filename|
  File.open(filename, 'w') { |f| f.write([1, 2, 3, 4].to_msgpack) }
end

Given /^a file named "(.*?)" with large msgpack data$/ do |filename|
  File.open(filename, 'w') { |f| f.write([1, 2, 3, 4].to_msgpack * 100_000) }
end

Then /^every file for "(.*?)" should be a valid msgpack stream$/ do |glob|
  Dir[glob].each do |filename|
    MessagePack::unpack(File.read(filename))
  end
end


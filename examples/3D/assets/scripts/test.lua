function update(deltaTime, node)
    if (deltaTime > 0.02) then
        print("Delta time: " .. deltaTime)
    end
    --print("Screen width: " .. getScreenWidth())
    --print("Node name: " .. node:getName())
end

-- Optional callback
function onKey(key, node)
    print("Key: " .. string.char(key))
    print("Node name: " .. node:getName())
end


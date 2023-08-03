function Update(deltaTime, node)
    if (deltaTime > 0.017) then
        print("LOW FPS! Delta time: " .. deltaTime)
    end
    --print("Screen width: " .. getScreenWidth())
    --print("Node name: " .. node:getName())
end

-- Optional callback
function OnKey(key, node)
    print("Key: " .. string.char(key))
    print("Node name: " .. node:getName())

    if (node:getComponent("Camera")) then
        print("Camera compo found!")
    end

    if (key == 114) then -- 114 is code for 'r' in SDL2/Villain::KeyCode
        print("Setting node pos")
        node:setPosition(0, 10, 0)
    end
end


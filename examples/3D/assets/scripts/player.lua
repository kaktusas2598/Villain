function Update(deltaTime, node)
    if (deltaTime > 0.017) then
        print("LOW FPS! Delta time: " .. deltaTime)
    end
    --print("Screen width: " .. getScreenWidth())
    --print("Node name: " .. node:getName())
end

music = GetAudio("assets/audio/TownTheme.mp3")
pitch = 1.0
-- Optional callback
function OnKey(key, node)
    if (node:getComponent("Camera")) then
        print("Camera compo found!")
    end

    --music = GetAudio("themeMusic")
    music:setVolume(0.02)
    music:setPitch(pitch)
    if (key == KeyCode("r")) then
        pitch = pitch - 0.01
        print("Setting node pos")
        local position = { x = 0.0, y = 20.0, z = 0.0 }
        node:setPosition(position)
        music:play()
    end
    local position = node:getPosition()
    print(position.x, position.y, position.z)
end

function OnCollide(node)
    print("OnCollide" .. math.random())
end

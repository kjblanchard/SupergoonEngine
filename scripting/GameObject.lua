local gameobject = {}
gameobject.Sprites = {}

function gameobject.Position(ptr)
    return cGameObject.Position(ptr)
end

function gameobject.SetPosition(ptr, x, y)
    return cGameObject.SetPosition(ptr, x, y)
end

function gameobject.Size(ptr)
    return cGameObject.Size(ptr)
end

function gameobject.SetSize(ptr, w, h)
    return cGameObject.SetSize(ptr, w, h)
end

function gameobject.Sprite(ptr)
    return gameobject.Sprites[ptr]
end

-- Calls the create function for the gameobject below ..
function gameobject.CreateGameObjectInCurrentMap()
    return cGameObject.CreateGameObject()
end

---Sets the gameobject type
---@param ptr userdata go ptr
---@param type integer The number of gameobject type, so it can update, start correctly
---@return nil
function gameobject.SetType(ptr, type)
    return cGameObject.SetType(ptr, type)
end

return gameobject

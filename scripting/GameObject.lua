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

return gameobject

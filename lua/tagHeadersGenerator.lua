-- SPDX-License-Identifier: GPL-3.0-only

local argparse = require "argparse"
local glue = require "glue"
local json = require "json"
local parser = require "tagDefinitionParser"

local argsParser = argparse()
argsParser:argument("outputDirectory", "Headers output directory"):args(1)
argsParser:argument("files", "Tag definitions files"):args("*")

local args = argsParser:parse()
local files = args.files
local outputDirectory = args.outputDirectory

---Generate a header file for a tag definition
---@param definitionName string
---@param tagDefinition ParsedTagDefinition
---@param dependencies table<string, table<string, boolean>>
local function generateHeader(definitionName, tagDefinition, dependencies)
    local structContent = ""

    local function add(text)
        structContent = structContent .. text
    end

    local function ident(n)
        add(string.rep(" ", n*4))
    end

    local function addType(typeName)
        add("T<" .. typeName .. ">")
    end

    add([[
// SPDX-License-Identifier: GPL-3.0-only
// This file is auto-generated. DO NOT EDIT!

#ifndef RINGWORLD__TAG__DEFINITIONS__]] .. definitionName:upper() .. [[_H
#define RINGWORLD__TAG__DEFINITIONS__]] .. definitionName:upper() .. [[_H

#include "../../types/types.h"
#include "../tag.h"
#include "enum.h"
#include "bitfield.h"

#pragma pack(push, 1)

]])

    if(dependencies) then
        for dependency, _ in pairs(dependencies) do
            if dependency ~= "common" and dependency ~= definitionName then
                add("#include \"" .. dependency .. ".h\"\n")
            end
        end
    end
    add("\n")

    for _, enum in ipairs(tagDefinition.enums) do
        local enumName = parser.toPascalCase(enum.name)
        local enumPrefix = parser.toSnakeCase(enum.name):upper() .. "_"
        add("typedef enum PACKED_ENUM " .. enumName .. " {\n")
        for index, value in ipairs(enum.values) do
            local valueName = enumPrefix .. parser.toSnakeCase(value.name):upper()
            ident(1)
            if index == 1 then
                add(valueName .. " = 0,\n")
            else
                add(valueName .. ",\n")
            end
        end
        local enum_count = enumPrefix .. "MAX_VALUE"
        ident(1)
        add(enum_count .. ", \n")
        local enum_size = enumPrefix .. "SIZE"
        ident(1)
        add(enum_size .. " = 0xFFFF \n")
        add("} " .. enumName .. ";\n\n")
    end

    for _, bitfield in ipairs(tagDefinition.bitfields) do
        local bitfieldType
        local width = bitfield.width
        if width == 8 then
            bitfieldType = "uint8_t"
        elseif width == 16 then
            bitfieldType = "uint16_t"
        else 
            bitfieldType = "uint32_t"
        end

        local bitfieldName = parser.toPascalCase(bitfield.name)
        add("typedef struct " .. bitfieldName .. " {\n")
        for _, field in ipairs(bitfield.fields) do
            ident(1)
            add(bitfieldType .. " " .. field.name .. " : 1;\n")
        end
        add("} " .. bitfieldName .. ";\n")
        add("_Static_assert(sizeof(" .. bitfieldName .. ") == sizeof(" .. bitfieldType .. "));\n\n")
    end
    
    for _, struct in pairs(tagDefinition.structs) do
        local structName = parser.toPascalCase(struct.name)
        ident(0)
        add("typedef struct " .. structName .. " { \n")
        if struct.inherits then
            ident(1)
            add(parser.toPascalCase(struct.inherits) .. " base; \n")
        end
        for _, field in ipairs(struct.fields) do
            ident(1)
            if not field.type then
                add("byte " .. field.name .. "[" .. field.size .. "];\n")
            elseif field.type == "Pad" then
                add("char pad_" .. string.len(structContent) .. "[" .. field.size .. "];\n")
            else
                if field.type == "TagBlock" then
                    add("struct { uint32_t count; struct " .. parser.toPascalCase(field.struct) .. " *elements; TagBlockDefinition *definition; }")
                else
                    add(field.type)
                end
                
                -- Field name
                add(" ")
                if field.pointer then
                    add("*")
                end
                add(field.name)
                if field.size then
                    add("[" .. field.size .. "]")
                end
                
                add(";\n")
            end
        end
        ident(0)
        add("} " .. structName .. ";\n")
        ident(0)
        add("_Static_assert(sizeof(" .. structName .. ") == " .. struct.size .. ");\n\n")
    end        
    add([[

#pragma pack(pop)

#endif

]])
    return structContent
end

local definitions = {}

for _, file in ipairs(files) do
    local fileName = file:match("([^/]+)$")
    local definitionName = fileName:match("^(.+)%..+$")
    local definition = json.decode(glue.readfile(file))
    local parsedDefinition = parser.parseTagDefinition(definition)
    definitions[definitionName] = parsedDefinition
end

local dependencies = parser.getDependenciesForTagDefinition(definitions)

for definitionName, definition in pairs(definitions) do
    -- generate and write header
    local definitionDependencies = dependencies[definitionName]

    -- Generate little endian header
    local header = generateHeader(definitionName, definition, definitionDependencies)
    glue.writefile(outputDirectory .. "/impl/tag/definitions/" .. definitionName .. ".h", header, "t")
end

﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (C) 2008-2016 Ryo Suzuki
//	Copyright (C) 2016 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <memory>
# include "Fwd.hpp"

namespace s3d
{
	enum class MMFOpenMode_IfExists
	{
		Fail,

		JustOpen,

		MapAll,

		Truncate,
	};

	enum class MMFOpenMode_IfNotFound
	{
		Fail,

		Create,
	};

	class WritableMemoryMapping
	{
	private:

		class CWritableMemoryMapping;

		std::shared_ptr<CWritableMemoryMapping> pImpl;

	public:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		WritableMemoryMapping();

		explicit WritableMemoryMapping(const FilePath& path,
			MMFOpenMode_IfExists ifExists = MMFOpenMode_IfExists::Truncate,
			MMFOpenMode_IfNotFound ifNotFound = MMFOpenMode_IfNotFound::Create)
			: WritableMemoryMapping()
		{
			open(path, ifExists, ifNotFound);
		}

		bool open(const FilePath& path,
			MMFOpenMode_IfExists ifExists = MMFOpenMode_IfExists::Truncate,
			MMFOpenMode_IfNotFound ifNotFound = MMFOpenMode_IfNotFound::Create);

		void close();

		bool isOpened() const;

		explicit operator bool() const
		{
			return isOpened();
		}

		void map(size_t offset = 0, size_t requestSize = 0);

		void unmap();

		bool flush();

		size_t offset() const;

		size_t mappedSize() const;

		int64 fileSize() const;

		uint8* data() const;

		const FilePath& path() const;
	};
}